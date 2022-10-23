add_library(myEvContent myEvContent.h myEvContent.cpp)
target_include_directories(myEvContent PRIVATE ".")

add_executable(dc dataCollector.cpp)
target_include_directories(dc PRIVATE ".")
target_link_libraries(dc myEvContent)
target_link_libraries(dc amqpcpp ev pthread dl ssl)

