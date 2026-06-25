include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(WYRM_IDL_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/idl-generated)

idlcxx_generate(TARGET wyrm-idl
    FILES
        idl/geometry_msgs/msg/Point.idl
        idl/geometry_msgs/msg/Quaternion.idl
        idl/Description.idl
        idl/RigidBody.idl
        idl/Frame.idl
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/idl
    OUTPUT_DIR ${WYRM_IDL_OUTPUT_DIR}
)

add_library(wyrm-cpp
    
)

target_sources(wyrm-cpp
    PRIVATE $<TARGET_PROPERTY:wyrm-idl,INTERFACE_SOURCES>
)

add_dependencies(wyrm-cpp wyrm-idl)

target_include_directories(wyrm-cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/..>
        $<BUILD_INTERFACE:${WYRM_IDL_OUTPUT_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(wyrm-cpp
    PUBLIC
        CycloneDDS-CXX::ddscxx
)

install(TARGETS wyrm-cpp
    EXPORT wyrm-cpp-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${WYRM_IDL_OUTPUT_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
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
