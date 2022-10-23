#include <myEvContent.h>

void MyHandler::onError(AMQP::TcpConnection *connection, const char *message){
    std::cout << "error: " << message << std::endl;
}

void MyHandler::onConnected(AMQP::TcpConnection *connection){
    std::cout << "connected" << std::endl;
}

void MyHandler::onReady(AMQP::TcpConnection *connection){
    std::cout << "ready" << std::endl;
}

void MyHandler::onClosed(AMQP::TcpConnection *connection){
    std::cout << "closed" << std::endl;
}

void MyHandler::onDetached(AMQP::TcpConnection *connection){
    std::cout << "detached" << std::endl;
}

MyHandler::MyHandler(struct ev_loop *loop) : AMQP::LibEvHandler(loop)
{}

MyHandler::~MyHandler() = default;

void closeChannelConnection(AMQP::TcpConnection *myConnection,
                                AMQP::TcpChannel *myChannel,
                                void (*cb)(void)){
    std::cout<<"closing channel...";
    myChannel->close()
        .onSuccess([myConnection, cb]() {                               
            // close the connection
            std::cout<<"closing connection...";
            // don't use deferred responses for connections
            myConnection->close(); 

            /** call callback function if it exits
                original intent is breaking libev's 
                loop to ensure neat termination.
                Do nothing if no fcnPtr is supplied **/
            if(cb != nullptr) cb();
        })
        .onError([](const char *message){
            std::cout << "closing channel failed:  " 
                << message <<std::endl;
        });
}

int publish(std::string address,
            std::string queue,
            std::vector<std::string> messages){
    int o = 0;
    if(messages.size()>0){
        // access to the event loop
        auto *loop = EV_DEFAULT;

        // init the SSL library
        #if OPENSSL_VERSION_NUMBER < 0x10100000L
            SSL_library_init();
        #else
            OPENSSL_init_ssl(0, NULL);
        #endif

        // handler for libev
        MyHandler handler(loop);

        // connection creation
        AMQP::TcpConnection connection(&handler, 
            AMQP::Address(address));
        
        // channel creation
        AMQP::TcpChannel channel(&connection);

        channel.declareQueue(queue)
            .onSuccess([&connection, &channel, &messages](){
                // publish a number of messages
                channel.startTransaction();

                for(auto it = messages.begin(); it!=messages.end(); it++) 
                    channel.publish("", "hello", *it);

                channel.commitTransaction()
                    .onSuccess([&connection, &channel]() {
                        closeChannelConnection(&connection, &channel);
                    })
                    .onError([](const char *message) {
                        // none of the messages were published 
                        // now we have to do it all over again
                        std::cout << "none of the messages were published;"<<
                        " now we have to do it all over again" << std::endl;
                    });

        })
            .onError([](const char *message){
                std::cout << "declaring queue failed:  " << message <<std::endl;
            });

        // run the loop
        ev_run(loop, 0);
    }
    return o;
}