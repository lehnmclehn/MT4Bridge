#include "AMQPConnection.h"
#include <thread>
#include <queue>
#include <stdio.h>
#include <string>
#include <sstream>
#include <json/json.h>
#include <iomanip>
#include <unistd.h>

std::queue<std::string> messageQueue;

bool  globalStop = false;

void die_on_amqp_error(amqp_rpc_reply_t reply, const char *s) {
    if (reply.library_error != 0) {
        printf("%s: %s", "Error occured\n", s);
        exit(0);
    }
}

void die_on_error(int error, const char *s) {
    if (error != 0) {
        printf("%s: %s", "Error occured\n", s);
        exit(0);
    }
}

void die(const char *s) {
    printf("%s\n", s);
    exit(0);
}


void AMQPConnection::init() {
    printf("Init function ...\n");

    globalStop = false;

    conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        die("creating TCP socket");
    }

    int status = amqp_socket_open(socket, hostname, port);
    if (status) {
        die("opening TCP socket");
    }

    die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                 "guest", "guest"),
                      "Logging in");
    amqp_channel_open(conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");


}

std::string convertToPrecision(float value, int precision) {
    std::ostringstream ss;
    ss << std::setprecision(precision) << ::std::fixed << value;
    return ss.str();
}

void AMQPConnection::send(TDHBar *bar) {
    Json::Value root;
    root["symbol"]= bar->symbol;
    root["time"]  = std::to_string(bar->time);
    root["period"]= std::to_string(bar->period);
    root["open"] = convertToPrecision( bar->open ,bar->digits);
    root["high"]  = convertToPrecision( bar->high ,bar->digits);
    root["low"]   = convertToPrecision( bar->low ,bar->digits);
    root["close"] = convertToPrecision( bar->close ,bar->digits);

    std::stringstream msg;
    msg << root;

    // If you like the default
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2; /* persistent delivery mode */
    die_on_error(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange),
                                    amqp_cstring_bytes(bindingKeyBar), 0, 0,
                                    &props, amqp_cstring_bytes(msg.str().c_str())),
                 "Publishing");
}

void AMQPConnection::send(TDHCmdResponse *data) {
    Json::Value root;
    root["id"]= data->id;
    root["state"]  = data->state;

    std::stringstream msg;
    msg << root;

    // If you like the default
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2; /* persistent delivery mode */
    die_on_error(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange),
                                    amqp_cstring_bytes(bindingKeyCmdResponse), 0, 0,
                                    &props, amqp_cstring_bytes(msg.str().c_str())),
                 "Publishing");
}

void AMQPConnection::deinit() {

    globalStop = true;
}


void receiveLoop(
        amqp_connection_state_t conn,
        const char *queue
) {
    amqp_basic_consume(conn, 1, amqp_cstring_bytes(queue), amqp_empty_bytes,
                       0, 1, 0, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Consuming");

    while (!globalStop) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(conn);

        timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        res = amqp_consume_message(conn, &envelope, &timeout, 0);

/*
        if (AMQP_RESPONSE_LIBRARY_EXCEPTION == res.reply_type) {
            if (AMQP_STATUS_TIMEOUT == res.library_error) {
                printf("library error --> das ist wirklich timeout \n");
            }
            printf("library error: %u --> %s\n", res.library_error, amqp_error_string(res.library_error));
        }*/

        if (AMQP_RESPONSE_NORMAL == res.reply_type) {
            char buf[1024];
            strncpy(buf, (const char *) envelope.message.body.bytes, envelope.message.body.len);
            buf[envelope.message.body.len] = 0;

            std::string el = buf;
            messageQueue.push(el);
            // printf("==== Message received ===\n%s\n", buf);
            // amqp_dump(envelope.message.body.bytes, envelope.message.body.len);

            amqp_destroy_envelope(&envelope);
        }

    }

    /* close channel + connection */
    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
                      "Closing channel");

    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
                      "Closing connection");

    amqp_destroy_connection(conn);
}

void jsonParseCmd(std::string json, TDHCmd *cmd) {
    std::stringstream jsonText(json);
    Json::Value root;
    jsonText >> root;

    std::string::size_type sz;
    strcpy(cmd->id, root.get("id", "?").asCString());
    strcpy(cmd->action, root.get("action", "?").asCString());
    strcpy(cmd->symbol, root.get("symbol", "?").asCString());
    cmd->dir = (root.get("dir", "L").asCString()[0] == 'L') ? 1 : 0;

    cmd->count = root.get("count", 0.0).asFloat();
    cmd->takeprofit = root.get("takeprofit", 0.0).asFloat();
    cmd->stopp = root.get("stopp", 0.0).asFloat();

    cmd->entrytime = root.get("entrytime", 0.0).asInt();
    cmd->entryprice = root.get("entryprice", 0.0).asFloat();
    cmd->exittime = root.get("exittime", 0.0).asInt();
    cmd->exitprice = root.get("exitprice", 0.0).asFloat();
}

bool AMQPConnection::getMessage(TDHCmd *cmd) {

    if (messageQueue.empty()) return false;

    std::string el = messageQueue.front();
    messageQueue.pop();

    jsonParseCmd(el, cmd);

    return true;
}

void AMQPConnection::startListen() {

    myThread = new std::thread(receiveLoop, conn, listenQueue);
}

void AMQPConnection::stopListen() {
    //myThread->
}

int AMQPConnection::messagesAvailable() {
    return messageQueue.size();
}

