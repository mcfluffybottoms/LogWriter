cmake_minimum_required(VERSION 3.10)
project(logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS ON)
if(BUILD_SHARED_LIBS)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

add_subdirectory(lib/logWriter)

add_executable(logger 
    app/worker/src/main.cpp
    app/worker/include/worker.hpp
    app/worker/src/worker.cpp
)

add_executable(data_analyzer 
    app/socket_reader/src/main.cpp
    app/socket_reader/src/data_analyzer.cpp
    app/socket_reader/include/data_analyzer.hpp
)


target_include_directories(logger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(logger logWriter)

option(INCLUDE_TESTS ON)
if(INCLUDE_TESTS)
    add_executable(runAppTests app/worker/tests/tests.cpp app/worker/include/worker.hpp app/worker/src/worker.cpp)
    target_compile_definitions(runAppTests PUBLIC
        RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
    )
    target_link_libraries(runAppTests PRIVATE logWriter testFramework)
endif()

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")