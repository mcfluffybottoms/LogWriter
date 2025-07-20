cmake_minimum_required(VERSION 3.10)
project(logWriter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS ON)
if(BUILD_SHARED_LIBS)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

add_library(logWriter ${LIB_TYPE} include/logWriter.h src/logWriter.cpp)
set_target_properties(logWriter PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(logWriter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_include_directories(logWriter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(logWriter PUBLIC LOGWRITER_EXPORTS)
else()
    target_compile_definitions(logWriter PUBLIC LOGWRITER_STATIC)
endif()

option(INCLUDE_EXAMPLE ON)
if(INCLUDE_EXAMPLE)
    add_executable(logStructExample example/logStruct.cpp)
    target_link_libraries(logStructExample PRIVATE logWriter)
endif()

option(INCLUDE_TESTS ON)
if(INCLUDE_TESTS)
    add_library(testFramework INTERFACE)
    target_include_directories(testFramework INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../testFramework
    )
    add_executable(runTests tests/tests.cpp)
    target_compile_definitions(runTests PUBLIC
        RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
    )
    target_link_libraries(runTests PRIVATE logWriter testFramework)
endif()

message(STATUS "logWriter build type: ${BUILD_SHARED_LIBS}")
message(STATUS "example is built: ${INCLUDE_EXAMPLE}")
message(STATUS "test is built: ${INCLUDE_TESTS}")