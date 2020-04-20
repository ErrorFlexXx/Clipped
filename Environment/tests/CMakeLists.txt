# Creates a CTest test out of every .cpp in this folder.

project(EnvironmentTest)

file( GLOB TEST_SOURCES *.cpp )
file( GLOB TEST_HEADER *.h)

foreach( testSourceFilePath ${TEST_SOURCES} )
    get_filename_component(testNameWE ${testSourceFilePath} NAME_WE)
    get_filename_component(testPath ${testSourceFilePath} PATH)
    string( REPLACE ${testPath} "" testName ${testNameWE} )
    add_executable( ${testName} ${testSourceFilePath} ${TEST_HEADER} )
    target_link_libraries(${testName} ClippedUtils ClippedEnvironment)
    add_test(NAME ${testName} COMMAND $<TARGET_FILE:${testName}>)
    message("Created test case ${testName}")
endforeach( testSourceFilePath ${APP_SOURCES} )
