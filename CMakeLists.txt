cmake_minimum_required(VERSION 2.8)
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)
project(json-cpp)

add_definitions(-D_UNICODE)

if(NOT MSVC)
    add_definitions(-std=c++11 -Wall -pedantic)
else()
    add_definitions(/W4 /WX)
endif()

include_directories(include third_party)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(tests)
