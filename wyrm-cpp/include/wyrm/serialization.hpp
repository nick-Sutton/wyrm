#pragma once

#include <vector>
#include "include/types.hpp"

std::vector<uint8_t> SerializeFrame(const WyrmFrame& frame);
std::vector<uint8_t> SerializeDescriptions(const DescriptionTable& descriptions);