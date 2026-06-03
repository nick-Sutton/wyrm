#include "include/types.hpp"
#include <cstring>

WyrmRigidBody ToWyrmBody(const sRigidBodyData& src,
                                 const DescriptionTable& descriptions) {
    WyrmRigidBody body{};
    body.id           = src.ID;
    body.x            = src.x;
    body.y            = src.y;
    body.z            = src.z;
    body.qx           = src.qx;
    body.qy           = src.qy;
    body.qz           = src.qz;
    body.qw           = src.qw;
    body.mean_error   = src.MeanError;
    body.tracking_lost = src.params & 0x01;
    body.model_filled  = src.params & 0x02;

    auto it = descriptions.find(src.ID);
    if (it != descriptions.end()) {
        std::strncpy(body.name, it->second.name, MAX_NAMELENGTH - 1);
    } else {
        std::snprintf(body.name, MAX_NAMELENGTH, "body_%d", src.ID);
    }
    return body;
}

WyrmFrame ToWyrmFrame(const sFrameOfMocapData& src,
                              const DescriptionTable& descriptions) {
    WyrmFrame frame{};
    frame.frame_id                           = src.iFrame;
    frame.timestamp                          = src.fTimestamp;
    frame.precision_timestamp_secs           = src.PrecisionTimestampSecs;
    frame.precision_timestamp_fractional_secs = src.PrecisionTimestampFractionalSecs;
    frame.is_recording                       = src.params & 0x01;
    frame.model_list_changed                 = src.params & 0x02;
    frame.body_count                         = src.nRigidBodies;

    frame.bodies.reserve(src.nRigidBodies);
    for (int i = 0; i < src.nRigidBodies; i++) {
        frame.bodies.push_back(ToWyrmBody(src.RigidBodies[i], descriptions));
    }
    return frame;
}

WyrmDescription ToWyrmDescription(const sRigidBodyDescription& src) {
    WyrmDescription description{};
    description.id          = src.ID;
    description.parent_id   = src.parentID;
    description.num_markers = src.nMarkers;
    std::strncpy(description.name, src.szName, MAX_NAMELENGTH - 1);
    return description;
}

void BuildDescriptionTable(sDataDescriptions* desc, WyrmContext& ctx) {
    for (int i = 0; i < desc->nDataDescriptions; i++) {
        if (desc->arrDataDescriptions[i].type == Descriptor_RigidBody) {
            WyrmDescription d = ToWyrmDescription(
                *desc->arrDataDescriptions[i].Data.RigidBodyDescription
            );
            ctx.descriptions[d.id] = d;
        }
    }
}

uint32_t PackBodyFlags(const WyrmRigidBody& b) {
    uint8_t flags = 0;
    if (b.tracking_lost) flags |= 0x01;
    if (b.model_filled)  flags |= 0x02;
    return flags;
}

uint32_t PackFrameFlags(const WyrmFrame& f) {
    uint8_t flags = 0;
    if (f.is_recording)       flags |= 0x01;
    if (f.model_list_changed) flags |= 0x02;
    return flags;
}

void UnpackBodyFlags(uint8_t flags, WyrmRigidBody& b) {
    b.tracking_lost = flags & 0x01;
    b.model_filled  = flags & 0x02;
}

void UnpackFrameFlags(uint8_t flags, WyrmFrame& f) {
    f.is_recording       = flags & 0x01;
    f.model_list_changed = flags & 0x02;
}