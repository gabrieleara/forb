# CMake Version Required
cmake_minimum_required(VERSION 3.2)

# Project Name
project(forb)

# Setting default compiling mode to Release
# To compile in Debug mode use cmake -DCMAKE_BUILD_TYPE=Debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    message(STATUS "The CMAKE_BUILD_TYPE variable has been set to the default value")
endif ()

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Each subdirectory contains a target
add_subdirectory(compiler)
add_subdirectory(library)
add_subdirectory(tests)
# add_subdirectory(profiler)

####################### DOXYGEN #######################

# Documentation will be built only in release mode
if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    # We specify that we want to build the docs
    option(BUILD_DOC "Build documentation" ON)

    # Checking if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # Set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/CMakeDoxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # Request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started for project ${CMAKE_PROJECT_NAME}")

        # Note the option ALL which allows to build the docs together with the application
        add_custom_target(doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen for project ${CMAKE_PROJECT_NAME}"
                VERBATIM)
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)

endif (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
