cmake_minimum_required(VERSION 3.9)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # Export symbols on windows

project(ClippedFilesystem VERSION 0.0.1 DESCRIPTION "Clipped Filesystem library.")

# Public Header of this library (later copied to e.g. /usr/local/include/Project_Name/):
set(${PROJECT_NAME}_PUBLIC_HEADER 
    include/${PROJECT_NAME}/cFile.h
    include/${PROJECT_NAME}/cBinFile.h
    include/${PROJECT_NAME}/cTextFile.h
    include/${PROJECT_NAME}/cConfigFile.h
    include/${PROJECT_NAME}/Archives/cVdfsFile.h
)

add_library(${PROJECT_NAME} ${CLIPPED_BUILD_TYPE}
    ${${PROJECT_NAME}_PUBLIC_HEADER}
    src/cFile.cpp
    src/cBinFile.cpp
    src/cTextFile.cpp
    src/cConfigFile.cpp
    src/Archives/cVdfsFile.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE ClippedUtils)

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
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Make Library "find_project-able" by cmake, if installed.
install(EXPORT ${PROJECT_NAME}Config DESTINATION share/${PROJECT_NAME}Config/cmake)

export(TARGETS ${PROJECT_NAME} FILE "${PROJECT_NAME}Config.cmake")
