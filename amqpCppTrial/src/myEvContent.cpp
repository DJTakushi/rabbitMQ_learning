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