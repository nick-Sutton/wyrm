cmake_minimum_required(VERSION 3.20)

# Compiler Options
set(WYRM_COMPILER "gcc" CACHE STRING "Compiler to use (clang or gcc)")
set_property(CACHE WYRM_COMPILER PROPERTY STRINGS clang gcc)
if(WYRM_COMPILER STREQUAL "clang")
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
elseif(WYRM_COMPILER STREQUAL "gcc")
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
else()
    message(FATAL_ERROR "Invalid compiler: ${WYRM_COMPILER}. Choose 'clang' or 'gcc'.")
endif()

project(wyrm CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build Options
option(WYRM_BUILD_DAEMON  "Build the wyrm daemon"       ON)
option(WYRM_BUILD_TESTS   "Build tests"                 OFF)

# FetchContent Dependencies
include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0
)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.15.0
)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.2
)

FetchContent_MakeAvailable(fmt Catch2 tomlplusplus argparse)

# External Deps
find_package(zenohc REQUIRED)
find_package(zenohcxx REQUIRED)
find_package(fastcdr REQUIRED)

if(WYRM_BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(wyrm-cpp)


if(WYRM_BUILD_DAEMON)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/wyrmd/cmake")
    find_package(NatNet REQUIRED)
    add_subdirectory(wyrmd)
endif()