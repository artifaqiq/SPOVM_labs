ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)

cmake_minimum_required(VERSION 2.8)	
									
project(mainprog)				

add_library(processinfo STATIC processinfo.cpp)

add_library(processmanager STATIC processmanager.cpp)

add_executable(mainprog mainprog.cpp)

target_link_libraries(mainprog processmanager processinfo)	


project(printpidprog)				

add_executable(printpidprog printpidprog.cpp)

target_link_libraries(printpidprog)	