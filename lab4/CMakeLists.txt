ADD_DEFINITIONS(
    -std=c++11
    -lpthread
    -pthread
)

cmake_minimum_required(VERSION 2.8)

add_library(semaphore SHARED semaphore.cpp)
add_library(shmemory SHARED shmemory.cpp)
add_library(thread STATIC thread.cpp)
add_library(serverftp STATIC serverftp.cpp)

project(client)
add_executable(client client.cpp)
target_link_libraries(client semaphore shmemory)

project(server)
add_executable(server server.cpp)
target_link_libraries(server semaphore shmemory thread serverftp pthread)
