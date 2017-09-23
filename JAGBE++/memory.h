#pragma once
#include <cstdint>
class memory
{
public:
    void setReg8(const uint8_t number, const uint8_t value);
    uint8_t getReg8(const uint8_t number) const;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint8_t a;
    uint8_t f;
    inline uint16_t hl() { return (h << 8) | l; }
    inline void hl(uint16_t val) {
        h = val >> 8;
        l = val & 0xFF;
    }
    uint16_t sp;
    uint16_t pc;
    memory(uint8_t* const bootRom);
    ~memory();
    uint8_t getMappedMemory(const uint16_t address) const;
    uint8_t getMappedMemory(const uint16_t addressIn, const bool OAMDMARead) const;
    void setMappedMemory(const uint16_t address, const uint8_t value);
    uint8_t ie;
    uint8_t m_if;
    void setR16(const uint8_t number, const uint8_t lowByte, const uint8_t highByte);
private:
    size_t romLength;
    bool sramEnabled;
    uint8_t getRomMemory(const size_t bank, const  uint16_t address) const;
    const size_t ROMBANKSIZE = 0x4000;
    size_t rombank;
    uint8_t* rom;
    uint8_t* oam;
    uint8_t* vram;
    uint8_t* wram;
    uint8_t* hram;
    uint8_t* m_bootRom;
    uint8_t m_mbcMode;
    void clearRom(const size_t size) noexcept;
    void setMappedMemory_common(const uint16_t address, const uint8_t value);
};
