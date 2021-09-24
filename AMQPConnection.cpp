#include "AMQPConnection.h"
#include <thread>
#include <queue>
#include <stdio.h>
#include <string.h>

int countOfMessages = 0;
std::queue<std::string> messageQueue;

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

void AMQPConnection::send(const TDHTransferHeader msg) {
    {
        amqp_basic_properties_t props;
        props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
        props.content_type = amqp_cstring_bytes("text/plain");
        props.delivery_mode = 2; /* persistent delivery mode */
        die_on_error(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange),
                                        amqp_cstring_bytes(bindingkey), 0, 0,
                                        &props, amqp_cstring_bytes(msg.msg)),
                     "Publishing");
    }
}

void AMQPConnection::deinit() {
    /* close channel + connection */
    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
                      "Closing channel");

    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
                      "Closing connection");

    amqp_destroy_connection(conn);
}


void receiveLoop(
        amqp_connection_state_t conn,
        const char *queue
        ) {
    amqp_basic_consume(conn, 1, amqp_cstring_bytes(queue), amqp_empty_bytes,
                       0, 1, 0, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Consuming");

    for (;;) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(conn);

        res = amqp_consume_message(conn, &envelope, NULL, 0);
        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            break;
        }
/*
        printf("Delivery %u, exchange %.*s routingkey %.*s\n",
               (unsigned) envelope.delivery_tag, (int) envelope.exchange.len,
               (char *) envelope.exchange.bytes, (int) envelope.routing_key.len,
               (char *) envelope.routing_key.bytes);

        if (envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
            printf("Content-type: %.*s\n",
                   (int) envelope.message.properties.content_type.len,
                   (char *) envelope.message.properties.content_type.bytes);
        } */

        // TODO: manually limited
        countOfMessages++;

        char buf[1024];
        strncpy(buf, (const char*)envelope.message.body.bytes, envelope.message.body.len);
        buf[envelope.message.body.len] = 0;

        std::string el = buf;
        messageQueue.push(el);
        // printf("==== Message received ===\n%s\n", buf);
        // amqp_dump(envelope.message.body.bytes, envelope.message.body.len);

        amqp_destroy_envelope(&envelope);
    }
}

bool AMQPConnection::getMessage(TDHTransferHeader *msg) {

    if (messageQueue.empty()) return false;

    std::string el = messageQueue.front();
    messageQueue.pop();

    msg->version = 1;
    msg->type = 0;
    strcpy(msg->msg, el.c_str());

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

