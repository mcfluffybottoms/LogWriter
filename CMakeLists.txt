cmake_minimum_required(VERSION 3.10)
project(logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# STATIC LINKING - not able to override with cmake .. -DBUILD_SHARED_LIBS=ON
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libs" FORCE)

add_subdirectory(lib/logWriter)

add_executable(logger 
    src/main.cpp
    include/worker.hpp
    src/worker.cpp
)

target_include_directories(logger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(logger logWriter)

option(INCLUDE_TESTS ON)
if(INCLUDE_TESTS)
    add_executable(runAppTests tests/tests.cpp include/worker.hpp src/worker.cpp)
    target_compile_definitions(runAppTests PUBLIC
        RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
    )
    target_link_libraries(runAppTests PRIVATE logWriter testFramework)
endif()