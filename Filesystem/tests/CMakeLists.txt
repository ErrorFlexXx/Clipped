# Creates a CTest test out of every .cpp in this folder.

project(FilesystemTest)

file( GLOB TEST_SOURCES *.cpp )
file( GLOB TEST_HEADER *.h)

foreach( testSourceFilePath ${TEST_SOURCES} )
    get_filename_component(testNameWE ${testSourceFilePath} NAME_WE)
    get_filename_component(testPath ${testSourceFilePath} PATH)
    string( REPLACE ${testPath} "" testName ${testNameWE} )
    add_executable( ${testName} ${testSourceFilePath} ${TEST_HEADER} )
    target_link_libraries(${testName} ClippedUtils ClippedFilesystem)
    add_test(NAME ${testName} COMMAND $<TARGET_FILE:${testName}>)
    message("Created test case ${testName}")
endforeach( testSourceFilePath ${APP_SOURCES} )

ADD_CUSTOM_COMMAND(TARGET testVdfsRead POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_CURRENT_SOURCE_DIR}/extractTest.vdfs
                       extractTest.vdfs
                   COMMENT "Copying 'extractTest.vdfs' file.")
