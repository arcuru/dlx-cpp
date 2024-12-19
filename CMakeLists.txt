cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)

project(dlx 
    VERSION 1.0
    LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

add_library(dlx STATIC
    dlx.cpp
)

target_include_directories(dlx
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)