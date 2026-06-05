include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(wyrm-cpp
    src/serialization.cpp
)

target_include_directories(wyrm-cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(wyrm-cpp
    PUBLIC
        fastcdr
        zenohcxx::zenohc
)

install(TARGETS wyrm-cpp
    EXPORT wyrm-cpp-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT wyrm-cpp-targets
    FILE wyrm-cpp-targets.cmake
    NAMESPACE wyrm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wyrm-cpp
)

configure_package_config_file(
    cmake/wyrm-cpp-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/wyrm-cpp-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wyrm-cpp
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/wyrm-cpp-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wyrm-cpp
)

if(WYRM_BUILD_TESTS)
    add_subdirectory(tests)
endif()