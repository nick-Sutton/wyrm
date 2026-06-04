#pragma once

#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "include/types.hpp"

void ConnectToServer(NatNetClient& client, const WyrmConfig& cfg);
void NATNET_CALLCONV frame_callback(sFrameOfMocapData* data, void* user_data);