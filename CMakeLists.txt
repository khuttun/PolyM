cmake_minimum_required(VERSION 3.8)

project(PolyM)

add_library(polym 
    Msg.cpp
    Msg.hpp
    Queue.cpp
    Queue.hpp)

target_compile_features(polym PUBLIC cxx_std_11)

find_package (Threads)
target_link_libraries (polym PUBLIC Threads::Threads)

# Test program
add_executable(testpolym Test.cpp)
target_link_libraries(testpolym polym)
