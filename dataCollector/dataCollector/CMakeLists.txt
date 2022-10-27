set(OPENSSL_ROOT_DIR "/rl/libs/openssl")
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)



add_library(myEvContent myEvContent.h myEvContent.cpp)
target_include_directories(myEvContent PRIVATE "."
                            "../../libs/AMQP-CPP/include"
                            "../../libs/libev"
                            "../../libs/openssl/include")
target_link_directories(myEvContent PUBLIC "../../libs/AMQP-CPP/build/bin")

add_executable(dc dataCollector.cpp)
target_include_directories(dc PRIVATE "."
                    "../../cereal/include"
                    "../../libs/AMQP-CPP/include"
                    "../../libs/libev"
                    "../../libs/openssl/include")
target_link_directories(dc PUBLIC "../../libs/AMQP-CPP/build/bin"
                    "../../libs/libev/.libs"
                    "../../libs/openssl")
target_link_libraries(dc myEvContent)
target_link_libraries(dc amqpcpp ev pthread dl  OpenSSL::SSL)

