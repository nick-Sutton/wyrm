/**
 * Copyright [2026] [Nicholas Sutton]
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdexcept>
#include "net.hpp"

/**
 * Converts a NatNet RigidBody to a Wyrm RigidBody
 * @param src NatNet RigidBody to convert
 * @param descriptions a map of descriptions to get the Rigidbody Name
 * @return a WyrmRigidBody with the same values as the NatNet RigidBody
 */
WyrmRigidBody ToWyrmBody(const sRigidBodyData& src,
                                 const std::unordered_map<int32_t, WyrmDescription>& descriptions) {
    WyrmRigidBody body{};
    body.id()                       = src.ID;
    body.position().x()             = src.x;
    body.position().y()             = src.y;
    body.position().z()             = src.z;
    body.orientation().x()          = src.qx;
    body.orientation().y()          = src.qy;
    body.orientation().z()          = src.qz;
    body.orientation().w()          = src.qw;
    body.mean_error()               = src.MeanError;
    body.tracking_lost()            = src.params & 0x01;
    body.model_filled()             = src.params & 0x02;

    auto it = descriptions.find(src.ID);
    if (it != descriptions.end()) {
        body.name() = it->second.name();
    } else {
        body.name() = "body_" + std::to_string(src.ID);
    }
    return body;
}

/**
 * Converts a NatNet Frame to a Wyrm Frame
 * @param src NatNet Frame to convert
 * @param descriptions a map of descriptions to get the Rigidbody Names
 * @return a WyrmFrame with the same values as the NatNet Frame
 */
WyrmFrame ToWyrmFrame(const sFrameOfMocapData& src,
                              const std::unordered_map<int32_t, WyrmDescription>& descriptions) {
    WyrmFrame frame{};
    frame.frame_id()                            = src.iFrame;
    frame.timestamp()                           = src.fTimestamp;
    frame.precision_timestamp_secs()            = src.PrecisionTimestampSecs;
    frame.precision_timestamp_fractional_secs() = src.PrecisionTimestampFractionalSecs;
    frame.is_recording()                        = src.params & 0x01;
    frame.model_list_changed()                  = src.params & 0x02;
    frame.body_count()                          = src.nRigidBodies;

    frame.bodies().reserve(src.nRigidBodies);
    for (int i = 0; i < src.nRigidBodies; i++) {
        frame.bodies().push_back(ToWyrmBody(src.RigidBodies[i], descriptions));
    }
    return frame;
}

/**
 * Converts a NatNet Description to a Wyrm Description
 * @param src NatNet description to convert
 * @return a WyrmDescription with the same values as the NatNet Description
 */ 
WyrmDescription ToWyrmDescription(const sRigidBodyDescription& src) {
    WyrmDescription description{};
    description.id()          = src.ID;
    description.parent_id()   = src.parentID;
    description.num_markers() = src.nMarkers;
    description.name() = src.szName;
    return description;
}

/**
 * Connects the Client to the NatNet Server
 * @param client client to connect
 * @param cfg config storing the connection params
 */
void ConnectToServer(NatNetClient& client, const WyrmConfig& cfg) {
    switch (client.Connect(cfg.ToNatNet())) {
        case ErrorCode_OK:               break;
        case ErrorCode_Network:          throw std::runtime_error("Unable to Connect to Server");
        case ErrorCode_InvalidArgument:  throw std::runtime_error("Invalid connection params");
        case ErrorCode_InvalidOperation: throw std::runtime_error("Invalid operation");
        default:                         throw std::runtime_error("Unknown NatNet error");
    }

    sServerDescription server_desc;
    client.GetServerDescription(&server_desc);

    if (!server_desc.bConnectionInfoValid)
        throw std::runtime_error("Invalid connection params. Server predates NatNet 3.0");
    if (!server_desc.HostPresent)
        throw std::runtime_error("Unable to Connect to Server. Host not present");

    if (server_desc.ConnectionMulticast &&
        cfg.connection_type != ConnectionType_Multicast)
            throw std::runtime_error("Invalid connection params. Connection Types do not match");

    if (!server_desc.ConnectionMulticast &&
        cfg.connection_type != ConnectionType_Unicast)
            throw std::runtime_error("Invalid connection params. Connection Types do not match");

    std::ostringstream ip_oss;
    ip_oss << static_cast<int>(server_desc.HostComputerAddress[0]) << '.'
           << static_cast<int>(server_desc.HostComputerAddress[1]) << '.'
           << static_cast<int>(server_desc.HostComputerAddress[2]) << '.'
           << static_cast<int>(server_desc.HostComputerAddress[3]);

    if (ip_oss.str() != cfg.server_address)
        throw std::runtime_error("Unable to Connect to Server. Host IP Addresses do not match");

    std::ostringstream mc_oss;
    mc_oss << static_cast<int>(server_desc.ConnectionMulticastAddress[0]) << '.'
           << static_cast<int>(server_desc.ConnectionMulticastAddress[1]) << '.'
           << static_cast<int>(server_desc.ConnectionMulticastAddress[2]) << '.'
           << static_cast<int>(server_desc.ConnectionMulticastAddress[3]);

    if (mc_oss.str() != cfg.multicast_address)
        throw std::runtime_error("Unable to Connect to Server. Multicast Addresses do not match");

    if (server_desc.ConnectionDataPort != cfg.server_data_port)
        throw std::runtime_error("Invalid connection params. Data ports do not match.");
}

/**
 * Processes a Frame of mocap data from the server. This function is called
 * on a seperate thread any time a new packet is received. It will convert the 
 * NatNet frame to a Wyrm Frame then push the Wyrm frame onto a queue of frame to be processed.
 * @param data NatNet Frame
 * @param user_data WyrmContext that stores the Frame queue
 */
void NATNET_CALLCONV frame_callback(sFrameOfMocapData* data, void* user_data) {
    
    auto* ctx = static_cast<WyrmContext*>(user_data);
    WyrmFrame frame;
    {
        // Aquire Description Lock
        std::lock_guard<std::mutex> desc_lock(ctx->descriptions_mutex);
        frame = ToWyrmFrame(*data, ctx->descriptions);
    }
    {
        // Aquire Data buffer lock
        std::lock_guard<std::mutex> lock(ctx->buffer_mutex);
        ctx->frame_buffer.push(std::move(frame));
    }
    // Notify other threads that we are finished
    ctx->buffer_cv.notify_one();
}