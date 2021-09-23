#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>

#include "library.h"

void die_on_amqp_error(amqp_rpc_reply_t reply, const char *s) {
    if (reply.library_error != 0) {
        printf("%s: %s", "Error occured", s);
        exit(0);
    }
}

void die_on_error(int error, const char *s) {
    if (error != 0) {
        printf("%s: %s", "Error occured", s);
        exit(0);
    }
}


void die(const char *s) {
    printf("%s", s);
    exit(0);
}

class AMQPConnection {
private:
    const char *hostname = "localhost";
    const int port = 5672;
    const char *exchange = "alex_ex";
    const char *bindingkey = "alex_key";
    const char *queue = "alex_queue";

    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;

public:
    void init() {
        printf("Init function ...");

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

    void send(const char *msg) {
        {
            amqp_basic_properties_t props;
            props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
            props.content_type = amqp_cstring_bytes("text/plain");
            props.delivery_mode = 2; /* persistent delivery mode */
            die_on_error(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange),
                                            amqp_cstring_bytes(bindingkey), 0, 0,
                                            &props, amqp_cstring_bytes(msg)),
                         "Publishing");
        }
    }

    int bind() {


        /** BINDING
        amqp_queue_bind(conn, 1, amqp_cstring_bytes(queue),
                        amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey),
                        amqp_empty_table);
        die_on_amqp_error(amqp_get_rpc_reply(conn), "Unbinding");
    **/



        return 0;
    }


    void deinit() {
        /* close channel + connection */
        die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
                          "Closing channel");

        die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
                          "Closing connection");

        amqp_destroy_connection(conn);
    }
};


EXTERN_DLL_EXPORT void sendMessage(char* msg) {
    AMQPConnection conn;

    conn.init();
    conn.send(msg);

    conn.deinit();
}


int main() {
    printf("Starting Test ...");

    AMQPConnection conn;

    conn.init();
    conn.send("New connection with class");

    conn.deinit();
    return 0;
}