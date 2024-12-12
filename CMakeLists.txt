cmake_minimum_required(VERSION 3.16)
project(brace)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BRACE_BUILD_TESTS "Build tests for the brace project" OFF)

add_library(${PROJECT_NAME}
    src/brace.cpp
    src/tokenizer.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC include)

if(BRACE_BUILD_TESTS)
    include(cmake/get_cpm.cmake)
    CPMAddPackage("gh:doctest/doctest#v2.4.11")
    enable_testing()
    add_executable(json_test test/json_test.cpp)
    target_compile_features(json_test PRIVATE cxx_std_17)
    target_link_libraries(json_test PRIVATE brace doctest)
    add_test(NAME json_test COMMAND json_test)
endif()
