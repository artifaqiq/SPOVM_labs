ADD_DEFINITIONS(
    -std=c++11
    -lpthread
    -pthread
)

cmake_minimum_required(VERSION 2.8)

project(lab5)
add_executable(lab5 main.cpp)
target_link_libraries(lab5 pthread)


