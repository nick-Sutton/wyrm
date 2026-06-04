add_library(wyrm-cpp
    src/types.cpp
    src/serialization.cpp
)

target_include_directories(wyrm-cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(wyrm-cpp
    PUBLIC
        fastcdr
        zenohcxx::zenohc
)