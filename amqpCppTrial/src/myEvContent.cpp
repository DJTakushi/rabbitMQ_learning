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
