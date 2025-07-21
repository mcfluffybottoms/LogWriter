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
    app/socket_listener/src/main.cpp
    app/socket_listener/src/data_analyzer.cpp
    app/socket_listener/include/data_analyzer.hpp
)


target_include_directories(logger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(logger logWriter)

option(INCLUDE_TESTS ON)
if(INCLUDE_TESTS)
    add_executable(runWorkerAppTests app/worker/tests/tests.cpp app/worker/include/worker.hpp app/worker/src/worker.cpp)
    add_executable(runSocketAppTests app/socket_listener/tests/tests.cpp app/socket_listener/src/data_analyzer.cpp app/socket_listener/include/data_analyzer.hpp)
    target_link_libraries(runWorkerAppTests PRIVATE logWriter testFramework)
    target_link_libraries(runSocketAppTests PRIVATE logWriter testFramework)
endif()