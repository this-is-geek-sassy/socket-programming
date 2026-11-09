cmake_minimum_required(VERSION 3.20)
project(Tether LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sprint 0 requires a clean Debug build: every warning is an error-level
# signal, and the address and undefined-behavior sanitizers run with the
# binary. Release builds stay free of sanitizer instrumentation.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -Wpedantic -fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(tether_net src/net/file_descriptor.cpp)
target_include_directories(tether_net PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(tether_peer src/peer/main.cpp)
add_executable(tether_rendezvous src/rendezvous/main.cpp)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(file_descriptor_test tests/file_descriptor_test.cpp)
target_link_libraries(file_descriptor_test PRIVATE tether_net GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(file_descriptor_test)
