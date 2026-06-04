#include <fastcdr/Cdr.h>
#include <fastcdr/FastBuffer.h>
#include "include/serialization.hpp"

std::vector<uint8_t> SerializeFrame(const WyrmFrame& frame) {
    eprosima::fastcdr::FastBuffer buffer;
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS);

    cdr << frame.frame_id;
    cdr << frame.timestamp;
    cdr << frame.precision_timestamp_secs;
    cdr << frame.precision_timestamp_fractional_secs;
    cdr << static_cast<uint32_t>(PackFrameFlags(frame)); // This is cast to a u32 for alignment
    cdr << frame.body_count;

    for (const auto& body : frame.bodies) {
        cdr << body.id;
        cdr.serialize_array(body.name, MAX_NAMELENGTH);
        cdr << body.x << body.y << body.z;
        cdr << body.qx << body.qy << body.qz << body.qw;
        cdr << body.mean_error;
        cdr << static_cast<uint32_t>(PackBodyFlags(body)); // This is cast to a u32 for alignment
    }

    return std::vector<uint8_t>(
        reinterpret_cast<const uint8_t*>(buffer.getBuffer()),
        reinterpret_cast<const uint8_t*>(buffer.getBuffer()) + cdr.get_serialized_data_length()
    );
}

std::vector<uint8_t> SerializeDescriptions(const DescriptionTable& descriptions) {
    eprosima::fastcdr::FastBuffer buffer;
    eprosima::fastcdr::Cdr cdr(buffer, eprosima::fastcdr::Cdr::LITTLE_ENDIANNESS);

    cdr << static_cast<uint32_t>(descriptions.size());
    for (const auto& [id, desc] : descriptions) {
        cdr << desc.id;
        cdr << desc.parent_id;
        cdr.serialize_array(desc.name, MAX_NAMELENGTH);
        cdr << desc.num_markers;
    }

    return std::vector<uint8_t>(
        reinterpret_cast<const uint8_t*>(buffer.getBuffer()),
        reinterpret_cast<const uint8_t*>(buffer.getBuffer()) + cdr.get_serialized_data_length()
    );
}