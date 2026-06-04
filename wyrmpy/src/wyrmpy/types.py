from typing import Final
from dataclasses import dataclass

@dataclass
class WyrmDescription():
    id:          int
    parent_id:   int
    name:        str
    num_markers: int

@dataclass
class WyrmRigidBody():
    id:             int
    name:           str
    x:              float
    y:              float
    z:              float
    qx:             float
    qy:             float
    qz:             float
    qw:             float
    mean_error:     float
    tracking_lost:  bool
    model_filled:   bool

@dataclass
class WyrmFrame():
    frame_id:               int
    timestamp:              float
    pts_secs:               int
    pts_frac:               int
    is_recording:           bool
    model_list_changed:     bool
    body_count:             int
    bodies:                 list[WyrmRigidBody]