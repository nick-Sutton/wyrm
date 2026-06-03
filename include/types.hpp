#pragma once

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include "zenoh.hxx"
#include "NatNetTypes.h"
#include "NatNetClient.h"

struct WyrmDescription {
    int32_t id;
    int32_t parent_id;
    char    name[MAX_NAMELENGTH];
    int32_t num_markers;

};

using DescriptionTable = std::unordered_map<int32_t, WyrmDescription>;

struct WyrmRigidBody {
    int32_t id;
    char    name[MAX_NAMELENGTH];
    float   x, y, z;
    float   qx, qy, qz, qw;
    float   mean_error;
    bool    tracking_lost;
    bool    model_filled;
};

struct WyrmFrame {
    int32_t  frame_id;
    double   timestamp;
    uint32_t precision_timestamp_secs;
    uint32_t precision_timestamp_fractional_secs;
    bool     is_recording;
    bool     model_list_changed;
    uint32_t body_count;
    std::vector<WyrmRigidBody> bodies;
};

struct WyrmContext {
    zenoh::Session*                 session;
    std::mutex                      buffer_mutex;
    std::mutex                      descriptions_mutex;
    std::condition_variable         buffer_cv;
    DescriptionTable                descriptions;
    std::queue<WyrmFrame>           frame_buffer;
};

struct WyrmConfig {
    uint16_t    server_command_port;
    uint16_t    server_data_port;
    std::string server_address;
    std::string local_address;
    std::string multicast_address;
    ConnectionType connection_type;

    sNatNetClientConnectParams ToNatNet() const {
        sNatNetClientConnectParams p;
        p.serverCommandPort = server_command_port;
        p.serverDataPort    = server_data_port;
        p.serverAddress     = server_address.c_str();
        p.localAddress      = local_address.c_str();
        p.multicastAddress  = multicast_address.c_str();
        p.connectionType    = connection_type;
        return p;
    }
};

WyrmRigidBody ToWyrmBody(const sRigidBodyData& src,
                                 const DescriptionTable& descriptions);
WyrmFrame ToWyrmFrame(const sFrameOfMocapData& src,
                              const DescriptionTable& descriptions);
WyrmDescription ToWyrmDescription(const sRigidBodyDescription& src);
void BuildDescriptionTable(sDataDescriptions* desc, WyrmContext& ctx);
uint32_t PackBodyFlags(const WyrmRigidBody& b);
uint32_t PackFrameFlags(const WyrmFrame& f);
void UnpackBodyFlags(uint8_t flags, WyrmRigidBody& b);
void UnpackFrameFlags(uint8_t flags, WyrmFrame& f);
