cmake_minimum_required(VERSION 3.16)

project(
    cpu_cache_simulator
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

add_library(
    cache_core
    src/CacheStatistics.cpp
    src/CacheSet.cpp
    src/SetAssociativeCache.cpp
    src/MissClassifier.cpp
    src/CacheHierarchy.cpp
    src/HierarchyStatistics.cpp
)

target_include_directories(
    cache_core
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(
    cache_simulator
    src/main.cpp
    src/TraceParser.cpp
    src/CacheVisualizer.cpp
    src/BenchmarkRunner.cpp
)

target_link_libraries(
    cache_simulator
    PRIVATE
    cache_core
)

add_executable(
    cache_tests
    tests/cache_tests.cpp
)

target_link_libraries(
    cache_tests
    PRIVATE
    cache_core
)

enable_testing()

add_test(
    NAME cache_behavior_tests
    COMMAND cache_tests
)

add_executable(
    trace_parser_tests
    tests/trace_parser_tests.cpp
    src/TraceParser.cpp
)

target_include_directories(
    trace_parser_tests
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME trace_parser_tests
    COMMAND trace_parser_tests
)

add_executable(
    hierarchy_tests
    tests/hierarchy_tests.cpp
)

target_link_libraries(
    hierarchy_tests
    PRIVATE
    cache_core
)

add_test(
    NAME hierarchy_tests
    COMMAND hierarchy_tests
)