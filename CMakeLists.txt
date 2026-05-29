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

# CMake Modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

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

# NatNet
find_package(NatNet REQUIRED)

# Core executable
add_executable(wyrm src/main.cpp)
target_include_directories(wyrm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(wyrm PRIVATE
    NatNet::NatNet
    fmt::fmt
    tomlplusplus::tomlplusplus
    argparse::argparse
)

target_include_directories(wyrm PRIVATE ${CMAKE_SOURCE_DIR})

# Tests
add_executable(tests test/test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)