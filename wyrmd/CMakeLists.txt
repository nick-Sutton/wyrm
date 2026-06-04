list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_executable(wyrmd
    src/main.cpp
    src/io.cpp
    src/net.cpp
)

target_include_directories(wyrmd
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(wyrmd
    PRIVATE
        wyrm-cpp
        NatNet::NatNet
        fmt::fmt
        tomlplusplus::tomlplusplus
        argparse::argparse
)