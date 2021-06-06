cmake_minimum_required(VERSION 3.8)

project(PolyM)

add_library(polym 
    Msg.cpp
    Msg.hpp
    Queue.cpp
    Queue.hpp)

target_compile_features(polym PUBLIC cxx_std_11)

# Test program
enable_testing()
add_executable(testpolym Test.cpp)
target_link_libraries(testpolym polym)
add_test(NAME PolyM COMMAND testpolym)
