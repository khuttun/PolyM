cmake_minimum_required(VERSION 3.8)

option(POLYM_BUILD_TEST "Build test with Polym" OFF)
option(POLYM_BUILD_DOCS "Build docs with Polym" ON)

project(PolyM)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

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

function(check_python_module module)
  execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import ${module}"
    RESULT_VARIABLE result
    ERROR_QUIET
  )
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Python module '${module}' not found")
  else()
    message(STATUS "Found Python module '${module}'")
  endif()
endfunction()

if(POLYM_BUILD_DOCS)
  find_package(Doxygen REQUIRED)
  find_package(Sphinx REQUIRED)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  check_python_module(breathe)
  check_python_module(sphinx_rtd_theme)
  set_property(
    TARGET polym
    PROPERTY PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_LIST_DIR}/include/polym
  )
  add_subdirectory(docs)
endif()
