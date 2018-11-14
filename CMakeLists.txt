# CMake Version Required
cmake_minimum_required(VERSION 3.6)

# Project Name
project(forb)

# The C++ Standard Version
set(CMAKE_CXX_STANDARD 11)

# Setting default compiling mode to Release
# To compile in Debug mode use cmake -DCMAKE_BUILD_TYPE=Debug
if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "" FORCE)
    message(STATUS "The CMAKE_BUILD_TYPE variable has been set to the default value")
endif()

message (STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Flags change depending on DEBUG or RELEASE compilation
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror --pedantic")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# Each subdirectory contains a target
add_subdirectory(compiler)
add_subdirectory(library)
add_subdirectory(tests)