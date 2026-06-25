/**
 * Copyright [2026] [Nicholas Sutton]
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#pragma once

#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "dds/dds.hpp"
#include <string>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <wyrm/aliases.hpp> 

/** A Context Holds all the data that percists during the wyrm runtime */
struct WyrmContext {
    std::mutex                                                  buffer_mutex;
    std::mutex                                                  descriptions_mutex;
    std::condition_variable                                     buffer_cv;
    std::unordered_map<int32_t, WyrmDescription>                descriptions;
    std::queue<WyrmFrame>                                       frame_buffer;
    dds::pub::DataWriter<WyrmDescription>*                      description_writer;
    dds::pub::DataWriter<WyrmFrame>*                            frame_writer;
};

/** A Config stores all the network configuration settings needed to connect to the Motive Server */
struct WyrmConfig {
    uint16_t    server_command_port;
    uint16_t    server_data_port;
    std::string server_address;
    std::string local_address;
    std::string multicast_address;
    ConnectionType connection_type;

    /** Converts a WyrmConfig to a NatNet ConnectParams Struct */
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

/**
 * Builds the Description table that maps RigidBody Id's to their Description
 * @param desc A collection of NatNet Descriptions
 * @param ctx the wyrm context that stores the descriptions map
 */
void BuildDescriptionTable(sDataDescriptions* desc, WyrmContext& ctx);