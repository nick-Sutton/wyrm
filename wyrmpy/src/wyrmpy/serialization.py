from ._parser import parse_frame, parse_descriptions
from .types import WyrmFrame, WyrmDescription

def deserialize_frame(data: bytes) -> WyrmFrame:
    return parse_frame(data)

def deserialize_descriptions(data: bytes) -> dict[int, WyrmDescription]:
    return parse_descriptions(data)