find_path(NatNet_INCLUDE_DIR
    NAMES NatNetClient.h NatNetCAPI.h NatNetTypes.h NatNetRequests.h NatNetRepeater.h
    HINTS $ENV{NATNET_SDK_ROOT}/include
)

find_library(NatNet_LIBRARY
    NAMES NatNet
    HINTS $ENV{NATNET_SDK_ROOT}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NatNet
    REQUIRED_VARS NatNet_LIBRARY NatNet_INCLUDE_DIR
)

if(NatNet_FOUND AND NOT TARGET NatNet::NatNet)
    add_library(NatNet::NatNet SHARED IMPORTED)
    set_target_properties(NatNet::NatNet PROPERTIES
        IMPORTED_LOCATION "${NatNet_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${NatNet_INCLUDE_DIR}"
    )
endif()