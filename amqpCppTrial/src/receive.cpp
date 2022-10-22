
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <myEvContent.h>




int main(int argc, char* argv[]){
    std::cout << "starting main()..." <<std::endl;

    // access to the event loop
    auto *loop = EV_DEFAULT;

    // handler for libev
    MyHandler handler(loop);

    // connection creation
    AMQP::TcpConnection connection(&handler, 
        AMQP::Address("amqp://guest:guest@localhost/"));
    
    // channel creation
    AMQP::TcpChannel channel(&connection);


    /*--------------Receive content--------------------*/
    // callback function that is called when the consume operation starts
    auto startCb = [](const std::string &consumertag) {

        std::cout << "consume operation started...";
    };

    // callback function that is called when the consume operation failed
    auto errorCb = [](const char *message) {

        std::cout << "consume operation failed" << std::endl;
    };

    // callback operation when a message was received
    auto messageCb = [&channel](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {

        std::cout << "message received: \"";
        std::cout << message.body() << "\"" << std::endl;
        // acknowledge the message
        channel.ack(deliveryTag);
    };

    // start consuming from the queue, and install the callbacks
    channel.consume("hello")
        .onReceived(messageCb)
        .onSuccess(startCb)
        .onError(errorCb);

    // run the loop
    ev_run(loop, 0);

    std::cout << "exiting" <<std::endl;
    return 0;
}