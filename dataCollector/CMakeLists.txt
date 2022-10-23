cmake_minimum_required(VERSION 3.10)
project(dataCollector
  VERSION 1.0
  DESCRIPTION "collects financial data from rabbitMQ server"
HOMEPAGE_URL "https://www.takushi.us")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(dataCollector)
