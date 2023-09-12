#pragma once

#include <cstddef>
#include <cstdint>

void YUYV2RGB(const uint8_t *yuv, uint8_t *rgb, size_t bytes);
