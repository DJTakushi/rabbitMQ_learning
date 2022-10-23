
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <myEvContent.h>

//https://stackoverflow.com/questions/61551289/libev-pass-argument-to-callback
struct my_io{
    ev_io io;
    int otherfd;
    void (*closeConnectionCallback)(void);
    AMQP::TcpConnection* myConnection;
    AMQP::TcpChannel* myChannel;
};
void breakEV_DEFAULT(){
    std::cout <<__PRETTY_FUNCTION__ << "() called"<<std::endl;
    ev_break (EV_DEFAULT);// break the default loop
    return;
};

// all watcher callbacks have a similar signature
// this callback is called when data is readable on stdin
static void cInCb (EV_P_ ev_io *w_, int revents)
{
    std::string s;
    std::cin >> s;
    struct my_io *w = (struct my_io *)w_;

    if(s=="exit"){
        std::cout<<"closing channel+connection..."<<std::endl;
        closeChannelConnection(w->myConnection, w->myChannel, w->closeConnectionCallback);
    }
    else{
        std::cout << "cin received: \""<<s<<"\"" << std::endl;
        std::string myAddress = "amqp://guest:guest@localhost/";
        std::string myRoutingKey = "request";
        publish(myAddress, myRoutingKey,{s});
    } 
}


int main(int argc, char* argv[]){
    std::cout << "starting main()..." <<std::endl;

    auto *loop = EV_DEFAULT;    // access to the event loop
    MyHandler handler(loop);    // handler for libev

    // connection creation
    AMQP::TcpConnection connection(&handler, 
        AMQP::Address("amqp://guest:guest@localhost/"));
    
    // channel creation
    AMQP::TcpChannel channel(&connection);

    /*--------------std::cin handler content--------------------*/
    struct my_io w;
    w.myChannel=&channel;
    w.myConnection=&connection;
    w.closeConnectionCallback=breakEV_DEFAULT;

    // init and set io event for gathering user inputs
    ev_io_init (&w.io, cInCb, 0, EV_READ);
    ev_io_start (loop, &w.io);
    /*--------------std::cin handler content END--------------------*/

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
    auto messageCb = [&channel](const AMQP::Message &message, 
                        uint64_t deliveryTag, 
                        bool redelivered) {

        std::cout << "message received: \"";
        std::cout << message.body() << "\"" << std::endl;
        // acknowledge the message
        channel.ack(deliveryTag);
    };
    /*--------------Receive content END-----------------*/

    std::string receiveQueueName = "reply";//queue to consume from
    channel.declareQueue(receiveQueueName)
        .onSuccess([&channel, &messageCb, &startCb, &errorCb, &receiveQueueName](){
        // start consuming from the queue, and install the callbacks
        channel.consume(receiveQueueName)
            .onReceived(messageCb)
            .onSuccess(startCb)
            .onError(errorCb);
        });

    ev_run(loop, 0);    // run ev's loop

    std::cout << "exiting" <<std::endl;
    return 0;
}