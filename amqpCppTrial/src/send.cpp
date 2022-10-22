/**
 *  based on AMQP-CPP's LibEV.cpp example
*   (@author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>)
**/
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <myEvContent.h>


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

    // run the loop
    ev_run(loop, 0);

    // done
    return 0;
}

