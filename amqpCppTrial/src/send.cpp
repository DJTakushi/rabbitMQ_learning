/**
 *  LibEV.cpp
 * 
 *  Test program to check AMQP functionality based on LibEV
 * 
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2015 - 2018 Copernica BV
 */

/**
 *  Dependencies
 */
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

/**
 *  Custom handler
 */
class MyHandler : public AMQP::LibEvHandler
{
private:
    /**
     *  Method that is called when a connection error occurs
     *  @param  connection
     *  @param  message
     */
    virtual void onError(AMQP::TcpConnection *connection, const char *message) override
    {
        std::cout << "error: " << message << std::endl;
    }

    /**
     *  Method that is called when the TCP connection ends up in a connected state
     *  @param  connection  The TCP connection
     */
    virtual void onConnected(AMQP::TcpConnection *connection) override 
    {
        std::cout << "connected" << std::endl;
    }

    /**
     *  Method that is called when the TCP connection ends up in a ready
     *  @param  connection  The TCP connection
     */
    virtual void onReady(AMQP::TcpConnection *connection) override 
    {
        std::cout << "ready" << std::endl;
    }

    /**
     *  Method that is called when the TCP connection is closed
     *  @param  connection  The TCP connection
     */
    virtual void onClosed(AMQP::TcpConnection *connection) override 
    {
        std::cout << "closed" << std::endl;
    }

    /**
     *  Method that is called when the TCP connection is detached
     *  @param  connection  The TCP connection
     */
    virtual void onDetached(AMQP::TcpConnection *connection) override 
    {
        std::cout << "detached" << std::endl;
    }
    
    
public:
    /**
     *  Constructor
     *  @param  ev_loop
     */
    MyHandler(struct ev_loop *loop) : AMQP::LibEvHandler(loop) {}

    /**
     *  Destructor
     */
    virtual ~MyHandler() = default;
};

/**
 *  Class that runs a timer
 */
class MyTimer
{
private:
    /**
     *  The actual watcher structure
     *  @var struct ev_io
     */
    struct ev_timer _timer;

    /**
     *  Pointer towards the AMQP channel
     *  @var AMQP::TcpChannel
     */
    AMQP::TcpChannel *_channel;

    /**
     *  Name of the queue
     *  @var std::string
     */
    std::string _queue;


    /**
     *  Callback method that is called by libev when the timer expires
     *  @param  loop        The loop in which the event was triggered
     *  @param  timer       Internal timer object
     *  @param  revents     The events that triggered this call
     */
    static void callback(struct ev_loop *loop, struct ev_timer *timer, int revents)
    {
        // retrieve the this pointer
        MyTimer *self = static_cast<MyTimer*>(timer->data);

        // publish a message
        self->_channel->publish("", self->_queue, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }

public:
    /**
     *  Constructor
     *  @param  loop
     *  @param  channel
     *  @param  queue
     */
    MyTimer(struct ev_loop *loop, AMQP::TcpChannel *channel, std::string queue) : 
        _channel(channel), _queue(std::move(queue))
    {
        // initialize the libev structure
        ev_timer_init(&_timer, callback, 0.005, 1.005);

        // this object is the data
        _timer.data = this;

        // and start it
        ev_timer_start(loop, &_timer);
    }
    
    /**
     *  Destructor
     */
    virtual ~MyTimer()
    {
        // @todo to be implemented
    }
};


void closeChannelConnection(AMQP::TcpConnection *myConnection,
                                AMQP::TcpChannel *myChannel){
    std::cout<<"closing channel...";
    myChannel->close()
        .onSuccess([myConnection]() {                               
            // close the connection
            std::cout<<"closing connection...";
            // don't use deferred responses for connections
            myConnection->close(); 
            std::cout<<"done."<<std::endl;
        })
        .onError([](const char *message){
            std::cout << "closing channel failed:  " 
                << message <<std::endl;
        });
}


/**
 *  Main program
 *  @return int
 */
int main(int argc, char* argv[])
{
    std::vector<std::string> myArgs;
    if(argc == 1) myArgs.push_back("default message");
    else for(int i = 1; i < argc; i++) myArgs.push_back(argv[i]);

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
        AMQP::Address("amqp://guest:guest@localhost/"));
    
    // channel creation
    AMQP::TcpChannel channel(&connection);

    std::cout<<"declaring Queue...";
    channel.declareQueue("hello")
        .onSuccess([&connection, &channel, &myArgs](){
            // publish a number of messages
            std::cout<<"publishing...";

            channel.startTransaction();

            for(auto it = myArgs.begin(); it!=myArgs.end(); it++) 
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

    // std::cout<<"closing channel...";
    // channel.close()
    //     .onSuccess([&connection, &channel]() {                               
    //         // close the connection
    //         std::cout<<"closing connection...";
    //         connection.close(); // don't use deferred responses here
    //     })
    //     .onError([](const char *message){
    //         std::cout << "closing channel failed:  " << message <<std::endl;
    //     });
    // std::cout<<"done."<<std::endl;


    // run the loop
    ev_run(loop, 0);

    // done
    return 0;
}

