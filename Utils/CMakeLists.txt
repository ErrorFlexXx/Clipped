cmake_minimum_required(VERSION 3.9)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # Export symbols on windows

project(ClippedUtils VERSION 0.0.1 DESCRIPTION "Clipped Utility library.")

# Public Header of this library (later copied to e.g. /usr/local/include/ClippedUtils/):
set(${PROJECT_NAME}_PUBLIC_HEADER 
    include/${PROJECT_NAME}/cOsDetect.h
    include/${PROJECT_NAME}/cLogger.h
    include/${PROJECT_NAME}/cMemory.h
    include/${PROJECT_NAME}/cPath.h
    include/${PROJECT_NAME}/cString.h
    include/${PROJECT_NAME}/cTime.h
    include/${PROJECT_NAME}/Allocators/cBlockAllocator.h
    include/${PROJECT_NAME}/Allocators/cStdHeapAllocator.h
    include/${PROJECT_NAME}/DataStructures/cBspTree.h
    include/${PROJECT_NAME}/DataStructures/cTree.h
)

add_library(${PROJECT_NAME} SHARED
    src/cString.cpp
    src/cPath.cpp
    src/cTime.cpp
    ${${PROJECT_NAME}_PUBLIC_HEADER}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${${PROJECT_NAME}_PUBLIC_HEADER}"
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>  # <prefix>/include/${PROJECT_NAME}
    PRIVATE
    src
    include/${PROJECT_NAME}
)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Config
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Make Library "find_project-able" by cmake, if installed.
install(EXPORT ${PROJECT_NAME}Config DESTINATION share/${PROJECT_NAME}Config/cmake)

export(TARGETS ${PROJECT_NAME} FILE "${PROJECT_NAME}Config.cmake")
