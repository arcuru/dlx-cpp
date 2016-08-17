# Basic CMake project
cmake_minimum_required(VERSION 2.8.2)

# Set default build type to Release
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Include the current directory in every build
set(CMAKE_INCLUDE_CURRENT_DIR ON)
 
set(proj dlx)

# Name the project
project(dlx CXX)

# Enable C++14 features on gcc/clang
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(GNU|Clang)")
    set(CMAKE_CXX_FLAGS "-std=c++14")
endif()

# Build executable from sources and headers
add_executable(nqueens dlx.cpp dlx.h dlx_containers.cpp dlx_containers.h test/nqueens.cpp)



