"""
Copyright [2026] [Nicholas Sutton]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0
"""

from ._crd_types import _CdrFrame, _CdrDescriptions
from .types import WyrmRigidBody, WyrmFrame, WyrmDescription

def deserialize_frame(data: bytes) -> WyrmFrame:
    """
    Deserializes a frame of motion capture data

    Args:
        data: The data to deserialize

    Returns:
        A WyrmFrame storing the deserialized data
    """
    raw = _CdrFrame.deserialize(data, has_header=False)
    return WyrmFrame(
        frame_id           = int(raw.frame_id),
        timestamp          = float(raw.timestamp),
        pts_secs           = int(raw.pts_secs),
        pts_frac           = int(raw.pts_frac),
        is_recording       = bool(raw.flags & 0x01),
        model_list_changed = bool(raw.flags & 0x02),
        body_count         = len(raw.bodies),
        bodies             = [
            WyrmRigidBody(
                id            = int(b.id),
                name          = b.name,
                x             = float(b.x),
                y             = float(b.y),
                z             = float(b.z),
                qx            = float(b.qx),
                qy            = float(b.qy),
                qz            = float(b.qz),
                qw            = float(b.qw),
                mean_error    = float(b.mean_error),
                tracking_lost = bool(b.flags & 0x01),
                model_filled  = bool(b.flags & 0x02),
            )
            for b in raw.bodies
        ]
    )

def deserialize_descriptions(data: bytes) -> dict[int, WyrmDescription]:
    """
    Deserializes Collection of RigidBody Descriptions

    Args:
        data: The data to deserialize

    Returns:
        A Dictionary that maps a Rigidbodies Id to its WyrmDescription
    """
    raw = _CdrDescriptions.deserialize(data, has_header=False)
    return {
        int(d.id): WyrmDescription(
            id          = int(d.id),
            parent_id   = int(d.parent_id),
            name        = d.name,
            num_markers = int(d.num_markers),
        )
        for d in raw.descriptions
    }
