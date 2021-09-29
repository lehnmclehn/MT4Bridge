#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <thread>
#include "library.h"

#ifndef AMPQ_CONNECTION_h
#define AMPQ_CONNECTION_h

/*
 *  Transfer command structure
 */

typedef struct {
    int version;
    int type;
    char msg[256];
} TDHTransferHeader;



class AMQPConnection {
private:
    const char *hostname = "localhost";
    const int port = 5672;

    // empty exchange means the AMQP default exchange (bindingKey ist the corresponding queue)
    const char *exchange = "";

    // defaults to the sending queue
    const char *bindingkey = "BAR";

    // queue where we get our commands
    const char *listenQueue = "CMD";

    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;

    std::thread *myThread = NULL;
public:
    /*
     * initializes the connection to AMQP server
     */
    void init();

    /*
     * send a message to a server
     */
    void send(const TDHTransferHeader msg);

    /*
     * loads a message from the connection
     * returns true if succesfully loaded
     */
    bool getMessage(TDHCmd* msg);

    /*
     * returns the amount of available messages
     */
    int messagesAvailable();

    /*
     * starts the receiving message server
     */
    void startListen();

    /*
     * stops the receiving message server
     */
    void stopListen();

    /*
     * closes all open connections for this instance
     */
    void deinit();
};

#endif
