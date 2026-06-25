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

#include <wyrm/aliases.hpp> 
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "context.hpp"

/**
 * Converts a NatNet RigidBody to a Wyrm RigidBody
 * @param src NatNet RigidBody to convert
 * @param descriptions a map of descriptions to get the Rigidbody Name
 * @return a WyrmRigidBody with the same values as the NatNet RigidBody
 */
WyrmRigidBody ToWyrmBody(const sRigidBodyData& src,
                                 const std::unordered_map<int32_t, WyrmDescription>& descriptions);

/**
 * Converts a NatNet Frame to a Wyrm Frame
 * @param src NatNet Frame to convert
 * @param descriptions a map of descriptions to get the Rigidbody Names
 * @return a WyrmFrame with the same values as the NatNet Frame
 */
WyrmFrame ToWyrmFrame(const sFrameOfMocapData& src,
                              const std::unordered_map<int32_t, WyrmDescription>& descriptions);

/**
 * Converts a NatNet Description to a Wyrm Description
 * @param src NatNet description to convert
 * @return a WyrmDescription with the same values as the NatNet Description
 */                           
WyrmDescription ToWyrmDescription(const sRigidBodyDescription& src);

/**
 * Connects the Client to the NatNet Server
 * @param client client to connect
 * @param cfg config storing the connection params
 */
void ConnectToServer(NatNetClient& client, const WyrmConfig& cfg);

/**
 * Processes a Frame of mocap data from the server. This function is called
 * on a seperate thread any time a new packet is received. It will convert the 
 * NatNet frame to a Wyrm Frame then push the Wyrm frame onto a queue of frame to be processed.
 * @param data NatNet Frame
 * @param user_data WyrmContext that stores the Frame queue
 */
void NATNET_CALLCONV frame_callback(sFrameOfMocapData* data, void* user_data);