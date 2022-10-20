cmake_minimum_required(VERSION 3.10)
project(takushiSend
  VERSION 1.0
  DESCRIPTION "send"
HOMEPAGE_URL "https://www.takushi.us")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

list(APPEND SOURCE_FILES myTcpHandler.cpp myTcpHandler.h send.cpp)
# target_include_directories(${PROJECT_NAME} PRIVATE .)
add_executable(iSend ${SOURCE_FILES})
list(APPEND MY_INCLUDE_DIRS "." "../AMQP-CPP/include")
target_include_directories(iSend PRIVATE ${MY_INCLUDE_DIRS})


# target_link_libraries(iSend -L/rl/amqpCppTrial/src/out/libamqpcppFAIL.a pthread dl)
target_link_libraries(iSend amqpcpp pthread dl)
