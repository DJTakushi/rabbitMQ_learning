cmake_minimum_required(VERSION 3.10)
project(takushiSend
  VERSION 1.0
  DESCRIPTION "send"
HOMEPAGE_URL "https://www.takushi.us")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(myEvContent myEvContent.h myEvContent.cpp)
target_include_directories(myEvContent PRIVATE ".")

add_executable(iSend send.cpp)
target_include_directories(iSend PRIVATE ".")
target_link_libraries(iSend myEvContent)
target_link_libraries(iSend amqpcpp ev pthread dl ssl)
