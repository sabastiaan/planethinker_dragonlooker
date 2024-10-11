cmake_minimum_required(VERSION 3.10)
project(JsonExpressionParser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add the main executable
add_executable(json_eval src/main.cpp src/parser.cpp)

# Specify include directories
target_include_directories(json_eval PRIVATE include)

# Add GTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# Add test executable
add_executable(json_tests tests/json_tests.cpp src/parser.cpp)
target_link_libraries(json_tests gtest gtest_main)
target_include_directories(json_tests PRIVATE include)

# Direct the tests and binaries to the bin folder
target_compile_definitions(json_tests PRIVATE "GTEST_BIN_DIR=\"${CMAKE_BINARY_DIR}/bin\"")
set_target_properties(json_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(GoogleTest)
gtest_discover_tests(json_tests)