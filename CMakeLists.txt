cmake_minimum_required(VERSION 3.9)

project(Clipped)

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

# Do not forget to target_link_libraries against ClippedUtils ClippedFilesystem ...
# Use (e.g.) #include <ClippedUtils/cLogger.h> and work in namespace Clipped to use this library.
