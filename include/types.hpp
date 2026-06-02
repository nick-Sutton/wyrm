#pragma once

const char *wyrm_pub_desc_keyexpr = "wyrm/pub/descriptions ";
const char *wyrm_pub_frame_keyexpr = "wyrm/pub/frame";

struct WyrmMarker {
    int32_t  id;
    float    x, y, z;
    float    size;
    float    residual;    // error in m/ray
    int16_t  params;      // bit 0=occluded, bit 4=unlabeled, bit 5=active
};

struct WyrmRigidBody {
    int32_t  id;
    char     name[MAX_NAMELENGTH];
    float    x, y, z;
    float    qx, qy, qz, qw;
    float    mean_error;
    int16_t  params;         // bit 0 = tracking lost, bit 1 = model filled
};

struct WyrmFrame {
    int32_t  frame_id;
    double   timestamp;
    uint32_t precision_ts_secs;
    uint32_t precision_ts_frac;
    int16_t  params;
    uint32_t body_count;
    uint32_t marker_count;
    std::vector<WyrmRigidBody> bodies;
    std::vector<WyrmMarker>   markers;
};

struct WyrmConfig {
    uint16_t serverCommandPort;
    uint16_t serverDataPort;
    std::string serverAddress;
    std::string localAddress;
    std::string multicastAddress;
    ConnectionType connectionType;

    // NatNet SDK parameter Struct
    sNatNetClientConnectParams toNatNet() const {
        sNatNetClientConnectParams p;
        p.serverCommandPort = serverCommandPort;
        p.serverDataPort    = serverDataPort;
        p.serverAddress     = serverAddress.c_str();
        p.localAddress      = localAddress.c_str();
        p.multicastAddress  = multicastAddress.c_str();
        p.connectionType    = connectionType;
        return p;
    }
};