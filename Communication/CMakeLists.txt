cmake_minimum_required(VERSION 3.9)

project(ClippedCommunication VERSION 0.0.1 DESCRIPTION "Clipped Communication library.")

# Public Header of this library:
set(${PROJECT_NAME}_PUBLIC_HEADER 
    include/${PROJECT_NAME}/cSerialPort.h
    include/${PROJECT_NAME}/cSerialPortLinux.h
    include/${PROJECT_NAME}/cSerialPortInterface.h
)

set(${PROJECT_NAME}_SOURCES
    src/cSerialPortLinux.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_PUBLIC_HEADER}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ClippedUtils)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${${PROJECT_NAME}_PUBLIC_HEADER}"
)

target_include_directories(${PROJECT_NAME} PUBLIC  
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
