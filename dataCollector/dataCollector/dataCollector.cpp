/**
 *  based on AMQP-CPP's LibEV.cpp example
*   (@author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>)
**/

#include <myEvContent.h>

    return;
};

int main(int argc, char* argv[])
{
    std::string myAddress = "amqp://guest:guest@localhost/";
    std::string myQueue = "reply";
    std::vector<std::string> myArgs;
    if(argc == 1){
        std::string defaultMessage = "default message";
        std::cout << "sending defaults"<<std::endl;
        std::cout << "  set address with:    \"-a[address]\"    (defult is '"<<myAddress<<"')"<<std::endl;
        std::cout << "  set queue with:    \"-q[queue]\"    (defult is '"<<myQueue<<"')"<<std::endl;
        std::cout << "  set messages with additional inputs   (default is '"<< defaultMessage << "!')"<<std::endl;
        myArgs.push_back(defaultMessage);
    }
    else{
        for(int i = 1; i < argc; i++){
            std::string s(argv[i]);
            if(s.size()>2){
                std::string h = s.substr(0,2);
                if(h=="-a"){
                    myAddress=s.substr(2,s.size()-2);
                    std::cout <<"myAddress set to:"<<myAddress<<std::endl;
                }
                else if(h=="-q"){
                    myQueue=s.substr(2,s.size()-2);
                    std::cout <<"myQueue set to:"<<myQueue<<std::endl;
                }
                else myArgs.push_back(s);
            }
            else myArgs.push_back(s);
        }
    } 
    publish(myAddress, myQueue, myArgs);

    return 0;//done
}

