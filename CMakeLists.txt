cmake_minimum_required(VERSION 3.10)
project(JsonExpressionParser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
# Force static linking for the entire project
# Ensure that the compiler includes debug symbols and disables optimizations in Debug mode
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
# set(CMAKE_EXE_LINKER_FLAGS "-static")
# Add the main executable
add_executable(json_eval src/main.cpp src/parser.cpp src/expression.cpp)

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
add_executable(json_tests tests/parser_tests.cpp tests/json_tests.cpp src/parser.cpp src/expression.cpp)
target_link_libraries(json_tests gtest gtest_main)
target_include_directories(json_tests PRIVATE include)

# Direct the tests and binaries to the bin folder
target_compile_definitions(json_tests PRIVATE "GTEST_BIN_DIR=\"${CMAKE_BINARY_DIR}/bin\"")
set_target_properties(json_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(GoogleTest)
gtest_discover_tests(json_tests)

# Add target to generate mock JSON files using an existing Python script if they don't exist
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/json_files/1KB.json ${CMAKE_BINARY_DIR}/json_files/100MB.json ${CMAKE_BINARY_DIR}/json_files/1GB.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/json_files
    COMMAND python ${CMAKE_SOURCE_DIR}/generate_json.py ${CMAKE_BINARY_DIR}/json_files/1KB.json 1024
        COMMAND python ${CMAKE_SOURCE_DIR}/generate_json.py ${CMAKE_BINARY_DIR}/json_files/100MB.json 100000000
    COMMAND python ${CMAKE_SOURCE_DIR}/generate_json.py ${CMAKE_BINARY_DIR}/json_files/1GB.json 10000000000
    COMMENT "Generating mock JSON files for benchmarking if they do not already exist..."
)

add_custom_target(generate_json_files DEPENDS ${CMAKE_BINARY_DIR}/json_files/1KB.json ${CMAKE_BINARY_DIR}/json_files/100MB.json ${CMAKE_BINARY_DIR}/json_files/1GB.json)

# Add a benchmark target with perf profiling
add_custom_target(run_benchmarks
    COMMAND ${CMAKE_COMMAND} --build . --target generate_json_files
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/perf_profiles
    COMMAND perf record -o ${CMAKE_BINARY_DIR}/perf_profiles/1KB_perf.data -- json_eval ${CMAKE_BINARY_DIR}/json_files/1KB.json `cat ${CMAKE_BINARY_DIR}/json_files/1KB.json.path`
    COMMAND perf record -o ${CMAKE_BINARY_DIR}/perf_profiles/100MB_perf.data -- json_eval ${CMAKE_BINARY_DIR}/json_files/100MB.json `cat ${CMAKE_BINARY_DIR}/json_files/100MB.json.path`
    COMMAND perf record -o ${CMAKE_BINARY_DIR}/perf_profiles/1GB_perf.data -- json_eval ${CMAKE_BINARY_DIR}/json_files/1GB.json `cat ${CMAKE_BINARY_DIR}/json_files/1GB.json.path`
    COMMENT "Running benchmarks with generated JSON files and recording perf profiles..."
)

# Add custom command to print instructions for loading the perf profiles
add_custom_target(print_perf_instructions
    COMMAND ${CMAKE_COMMAND} -E echo "To load the performance profiles, use the following commands:"
    COMMAND ${CMAKE_COMMAND} -E echo "  perf report -i ${CMAKE_BINARY_DIR}/perf_profiles/1KB_perf.data"
    COMMAND ${CMAKE_COMMAND} -E echo "  perf report -i ${CMAKE_BINARY_DIR}/perf_profiles/100MB_perf.data"
    COMMAND ${CMAKE_COMMAND} -E echo "  perf report -i ${CMAKE_BINARY_DIR}/perf_profiles/1GB_perf.data"
    COMMENT "Printing instructions to load perf profiles..."
)
