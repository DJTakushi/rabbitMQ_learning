#include <myTcpHandler.h>
#include <iostream>
int main(){
    // create an instance of your own tcp handler
    MyTcpHandler myHandler;

    // address of the server
    AMQP::Address address("amqp://guest:guest@localhost/vhost");

    // create a AMQP connection object
    AMQP::TcpConnection connection(&myHandler, address);

    // and create a channel
    AMQP::TcpChannel channel(&connection);

    // use the channel object to call the AMQP method you like
    channel.declareExchange("my-exchange", AMQP::fanout);
    channel.declareQueue("my-queue");
    channel.bindQueue("my-exchange", "my-queue", "my-routing-key");
    std::cout << "exiting."<<std::endl;
    return 0;
}