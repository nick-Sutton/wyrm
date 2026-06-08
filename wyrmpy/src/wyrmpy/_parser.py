# wyrmpy/src/wyrmpy/_parser.py
import struct
from .types import WyrmFrame, WyrmRigidBody, WyrmDescription

def _read(data: bytes, fmt: str, offset: int):
    size = struct.calcsize(fmt)
    value = struct.unpack_from('<' + fmt, data, offset)
    return (value[0] if len(value) == 1 else value), offset + size

def _read_string(data: bytes, offset: int) -> tuple[str, int]:
    length, offset = _read(data, 'I', offset)
    s = data[offset:offset + length - 1].decode('utf-8')
    offset += length
    offset = (offset + 3) & ~3                        # align to 4-byte boundary
    return s, offset

def parse_frame(data: bytes) -> WyrmFrame:
    offset = 0
    frame_id, offset   = _read(data, 'i', offset)   # 0-3
    timestamp, offset  = _read(data, 'd', offset)   # 4-11
    pts_secs, offset   = _read(data, 'I', offset)   # 12-15
    pts_frac, offset   = _read(data, 'I', offset)   # 16-19
    flags, offset      = _read(data, 'I', offset)   # 20-23
    body_count, offset = _read(data, 'I', offset)   # 24-27

    bodies = []
    for _ in range(body_count):
        id_, offset        = _read(data, 'i', offset)   # 28-31
        name, offset       = _read_string(data, offset) # 32: length, 36: chars
        x, offset          = _read(data, 'f', offset)
        y, offset          = _read(data, 'f', offset)
        z, offset          = _read(data, 'f', offset)
        qx, offset         = _read(data, 'f', offset)
        qy, offset         = _read(data, 'f', offset)
        qz, offset         = _read(data, 'f', offset)
        qw, offset         = _read(data, 'f', offset)
        mean_error, offset = _read(data, 'f', offset)
        bflags, offset     = _read(data, 'I', offset)
        bodies.append(WyrmRigidBody(
            id=id_, name=name, x=x, y=y, z=z,
            qx=qx, qy=qy, qz=qz, qw=qw,
            mean_error=mean_error,
            tracking_lost=bool(bflags & 0x01),
            model_filled=bool(bflags & 0x02),
        ))

    return WyrmFrame(
        frame_id=frame_id, timestamp=timestamp,
        pts_secs=pts_secs, pts_frac=pts_frac,
        is_recording=bool(flags & 0x01),
        model_list_changed=bool(flags & 0x02),
        body_count=body_count, bodies=bodies,
    )

def parse_descriptions(data: bytes) -> dict[int, WyrmDescription]:
    offset = 0
    count, offset = _read(data, 'I', offset)
    result = {}
    for _ in range(count):
        id_, offset         = _read(data, 'i', offset)
        parent_id, offset   = _read(data, 'i', offset)
        name, offset        = _read_string(data, offset)
        num_markers, offset = _read(data, 'i', offset)
        result[id_] = WyrmDescription(id=id_, parent_id=parent_id,
                                      name=name, num_markers=num_markers)
    return result