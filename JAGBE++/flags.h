#pragma once;
#include <cstdint>

namespace flags {
    constexpr uint8_t ZEROBIT = 0x80;
    constexpr uint8_t ZEROFLAG = 0x7;
    constexpr uint8_t NEGATIVEVEBIT = 0x40;
    constexpr uint8_t NEGATIVEFLAG = 0x6;
    constexpr uint8_t HALFBIT = 0x20;
    constexpr uint8_t HLAFFLAG = 0x5;
    constexpr uint8_t CARRYBIT = 0x10;
    constexpr uint8_t CARRYFLAG = 0x4;
    constexpr uint8_t getFlags(const bool zf, const bool nf, const bool hf, const bool cf) {
        return (zf ? ZEROBIT : 0) | (nf ? NEGATIVEVEBIT : 0) | (hf ? HALFBIT : 0) | (cf ? CARRYBIT : 0);
    }
    constexpr uint8_t f_getHC8(const uint8_t a, const uint8_t b) {
        return (((a & 0x0F) + (b & 0x0F)) & 0x10) == 0x10 ? flags::HALFBIT : 0;
    }

    constexpr uint8_t f_getHCN8(const uint8_t a, const uint8_t b) {
        return (a & 0xF) - (b & 0xF) < 0 ? flags::HALFBIT : 0;
    }

    constexpr uint8_t f_getZF8(const uint8_t val) {
        return val == 0 ? ZEROBIT : 0;
    }
    constexpr uint8_t f_getCF8(const uint8_t a, const uint8_t b) {
        return a + b > 0xFF ? CARRYBIT : 0;
    }
}
