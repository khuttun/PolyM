cmake_minimum_required(VERSION 3.8)

option(POLYM_BUILD_TEST "Build test with Polym" OFF)

project(PolyM)

add_library(polym 
    src/Msg.cpp
    src/Queue.cpp

    include/polym/Msg.hpp
    include/polym/Queue.hpp
)

target_compile_features(polym PUBLIC cxx_std_11)

find_package(Threads)
target_link_libraries(polym PUBLIC Threads::Threads)
target_include_directories(polym PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/)

# Test program
if(POLYM_BUILD_TEST)
  enable_testing()
  add_executable(testpolym test/Test.cpp)
  target_link_libraries(testpolym polym)
  target_include_directories(testpolym PRIVATE test/)
  add_test(NAME PolyM COMMAND testpolym)
endif()