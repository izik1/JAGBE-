#pragma once
#include <cstdint>
constexpr uint8_t bit(const uint8_t b) { return 1 << b; }
constexpr uint8_t nbit(const uint8_t b) { return ~(1 << b); }
