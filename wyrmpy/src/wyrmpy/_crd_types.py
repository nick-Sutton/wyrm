from typing import Final

from pycdr2 import IdlStruct
from pycdr2.types import int32, uint32, float32, float64, uint8, sequence
from dataclasses import dataclass
from .types import RigidBody, Frame, Description

MAX_NAME_LENGTH: Final = 256

@dataclass
class _CdrRigidBody(IdlStruct):
    id:         int32
    name:       uint8[MAX_NAME_LENGTH]
    x:          float32
    y:          float32
    z:          float32
    qx:         float32
    qy:         float32
    qz:         float32
    qw:         float32
    mean_error: float32
    flags:      uint32

@dataclass
class _CdrFrame(IdlStruct):
    frame_id:   int32
    timestamp:  float64
    pts_secs:   uint32
    pts_frac:   uint32
    flags:      uint32
    body_count: uint32
    bodies:     sequence[_CdrRigidBody]

def deserialize_frame(data: bytes) -> Frame:
    raw = _CdrFrame.deserialize(data) # deserialize with crd

    # Convert cdr type into wyrmpy types
    return Frame(
        frame_id           = int(raw.frame_id),
        timestamp          = float(raw.timestamp),
        pts_secs           = int(raw.pts_secs),
        pts_frac           = int(raw.pts_frac),
        is_recording       = bool(raw.flags & 0x01),
        model_list_changed = bool(raw.flags & 0x02),
        bodies             = [
            RigidBody(
                id            = int(b.id),
                name          = bytes(b.name).split(b'\x00', 1)[0].decode('utf-8'),
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