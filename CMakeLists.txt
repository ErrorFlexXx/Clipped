cmake_minimum_required(VERSION 3.9)

project(Clipped)

#Copy libraries next to executables.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CLIPPED_BUILD_STATIC OFF CACHE BOOL "Build shared libraries, if off, static libs otherwise.")
set(CLIPPED_BUILD_MATHS OFF CACHE BOOL "Build ClippedMaths library.")
set(CLIPPED_BUILD_FILESYSTEM OFF CACHE BOOL "Build ClippedFilesystem library.")
set(CLIPPED_BUILD_ENVIRONMENT OFF CACHE BOOL "Build ClippedEnvironment library.")
set(CLIPPED_BUILD_DATASTREAMS OFF CACHE BOOL "Build ClippedDatastreams library.")
set(CLIPPED_BUILD_COMMUNICATION OFF CACHE BOOL "Build ClippedCommunication library.")
set(CLIPPED_BUILD_ECS OFF CACHE BOOL "Build ClippedECS library.")
set(CLIPPED_BUILD_TESTS OFF CACHE BOOL "Build tests.")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Note: Build type static if only allowed for OSS projects!
if(CLIPPED_BUILD_STATIC)
    message("Clipped will be built for static lib usage.")
    set(CLIPPED_BUILD_TYPE STATIC)
else()
    message("Clipped will be built for shared lib usage.")
    set(CLIPPED_BUILD_TYPE SHARED)
endif()

# The following components of Clipped library are mandatory:
add_subdirectory(Utils)

# The following components of Clipped libraries are optional:
if(CLIPPED_BUILD_MATHS)
    add_subdirectory(Maths)
endif()

if(CLIPPED_BUILD_FILESYSTEM)
    add_subdirectory(Filesystem)
endif()

if(CLIPPED_BUILD_ENVIRONMENT)
    add_subdirectory(Environment)
endif()

if(CLIPPED_BUILD_DATASTREAMS)
    add_subdirectory(DataStreams)
endif()

if(CLIPPED_BUILD_COMMUNICATION)
    add_subdirectory(Communication)
endif()

if(CLIPPED_BUILD_ECS)
    add_subdirectory(ECS)
endif()

# Add tests (has to be made here instead of in the subdirectories, because
# they depend on the build libraries.
if(CLIPPED_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Utils/tests)
    add_subdirectory(Filesystem/tests)
endif()

# Do not forget to target_link_libraries against ClippedUtils ClippedFilesystem ...
# Use (e.g.) #include <ClippedUtils/cLogger.h> and work in namespace Clipped to use this library.
