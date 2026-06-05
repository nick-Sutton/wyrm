#pragma once

#include <vector>
#include <unordered_map>
#include <wyrm/types.hpp>

std::vector<uint8_t> SerializeFrame(const WyrmFrame& frame);
std::vector<uint8_t> SerializeDescriptions(const std::unordered_map<int32_t, WyrmDescription>& descriptions);

WyrmFrame DeserializeFrame(const std::vector<uint8_t>& data);
std::unordered_map<int32_t, WyrmDescription> DeserializeDescriptions(const std::vector<uint8_t>& data);