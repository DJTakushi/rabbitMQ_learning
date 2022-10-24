
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <myEvContent.h>

// wrapping ev_io with content that points to callback function 
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

class queueListener{
public:
    struct ev_loop* loop;
    AMQP::TcpConnection* connection;
    AMQP::TcpChannel* myChannel;
    struct my_io w; //this must be persistent

    queueListener(  struct ev_loop* loop_i,
                    AMQP::TcpConnection* connection_i,
                    AMQP::TcpChannel* channel_i){
        loop = loop_i;
        connection = connection_i;
        myChannel=channel_i;
        //std::cout <<"channel_i = "<< channel_i <<std::endl;//debugging

        /*--------------std::cin handler content--------------------*/
        w.myChannel=myChannel;
        w.myConnection=connection;
        w.closeConnectionCallback=breakEV_DEFAULT;

        // init and set io event for gathering user inputs
        ev_io_init (&w.io, cInCb, 0, EV_READ);
        ev_io_start (loop, &w.io);
        /*--------------std::cin handler content END--------------------*/

        auto startCb = [](const std::string &consumertag) {
            std::cout << "consume operation started...";
        };

        // callback function that is called when the consume operation failed
        auto errorCb = [](const char *message) {
            std::cout << "consume operation failed" << std::endl;
        };

        std::string receiveQueueName = "reply";//queue to consume from
        myChannel->declareQueue(receiveQueueName)
            .onSuccess([channel_i, /*&messageCb,*/ &startCb, &errorCb, &receiveQueueName](){
                // start consuming from the queue, and install the callbacks
                // std::cout <<"channel_i (declareQueue.onSuccess) = "<< channel_i <<std::endl;
                channel_i->consume(receiveQueueName)
                    .onReceived(
                        [channel_i](const AMQP::Message &message,uint64_t deliveryTag,bool redelivered) {
                        std::cout << "message received: \"";
                        std::cout << message.body() << "\", ";
                        // acknowledge the message
                        // std::cout <<"channel_i(messageCb) = "<< channel_i <<std::endl;
                        channel_i->ack(deliveryTag);
                        }
                        /** todo: 
                        call a queueListener function to handle this in more
                        modular fashion.  I tried doing this like 'startCb' and
                        'errorCb' below, but I wasn unable to correctly capture
                        the channel variable.  Pehaps using the pointer to the 
                        queueListener will work, but again, my initial attempts
                        to do so failed.  
                        **/
                        )
                    .onSuccess(startCb)
                    .onError(errorCb);
            });

        std::cout <<"queueListener initalized"<<std::endl;
    }

// private:
};


int main(int argc, char* argv[]){
    std::cout << "starting main()..." <<std::endl;

    auto *loop = EV_DEFAULT;    // access to the event loop
    MyHandler handler(loop);    // handler for libev

    // connection creation
    AMQP::TcpConnection connection(&handler, 
        AMQP::Address("amqp://guest:guest@localhost/"));
    
    // channel creation
    AMQP::TcpChannel channel(&connection);

    queueListener* myQueueListener = new queueListener(loop, &connection, &channel);

    ev_run(loop, 0);    // run ev's loop

    std::cout << "exiting" <<std::endl;
    return 0;
}