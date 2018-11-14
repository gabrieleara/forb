project(libforb)

# Needed to export the shared library in standard directories
include(GNUInstallDirs)

# The version of the shared library
set(FORB_LIB_VERSION 0.1)
set(FORB_LIB_SOVERSION 0)

# Our target is the libforb.so
add_library(forb SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/stream/shared_memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stream/socket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base_skeleton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base_stub.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/remote_registry.cpp)

# Setting the version of the shared library
set_target_properties(forb PROPERTIES
        VERSION ${FORB_LIB_VERSION}
        SOVERSION ${FORB_LIB_SOVERSION})

# "include" directory will be exported
target_include_directories(forb 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)

# PThread is required to link this library
target_link_libraries(forb
        PRIVATE pthread)

# This will install in /usr/lib and /usr/include
set(CMAKE_INSTALL_PREFIX /usr)

# Specifying what to export when installing
install(TARGETS forb
        EXPORT forb-export
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}) # Windows specific, might be useless

# Actually to export include directory we need to copy manually files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/forb)




