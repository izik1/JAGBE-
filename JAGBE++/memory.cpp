#include "memory.h"
#include <new>
#include <cstdint>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "jagbe++exceptions.h"
#define UNIMPLEMENTEDMEMORYOPERATION(operation, location, offset) do { \
    std::cerr << __FUNCTION__ << "::unimplemented::"<< (operation) << "::" << (location) << "::" << \
    std::setfill('0') << std::setw(4) << std::hex <<  (offset) << std::endl; \
} while(0)

#define UNIMPLEMENTEDWRITE(location, offset) UNIMPLEMENTEDMEMORYOPERATION("WRITE", (location), (offset))
#define UNIMPLEMENTEDREAD(location, offset) do{ UNIMPLEMENTEDMEMORYOPERATION("READ", (location), (offset)); return 0xFF; } while(0)

constexpr bool usesMainBus(const uint16_t address) {
    return address < 0x8000 || (address >= 0xA000 && address < 0xFE00);
}
constexpr bool usesVRam(const uint16_t address) {
    return address >= 0x8000 && address < 0xA000;
}

constexpr bool hasBusConflict(const uint16_t a1, const uint16_t a2) {
    return  (usesMainBus(a1) && usesMainBus(a2)) || (usesVRam(a1) && usesVRam(a2));
}

memory::memory(uint8_t* const p_bootRom) {
    assert(p_bootRom);
    const size_t ROMSIZE = 0x8000;
    const size_t WRAMSIZE = 0x2000;
    const size_t VRAMSIZE = 0x2000;
    this->rom = new uint8_t[ROMSIZE];
    this->oam = new uint8_t[0xA0];
    this->wram = new uint8_t[WRAMSIZE];
    this->vram = new uint8_t[VRAMSIZE];
    this->hram = new uint8_t[0x7F];
    this->m_bootRom = p_bootRom;
    this->rombank = 1;
    this->ie = 0;
    this->m_if = 0;
    this->m_mbcMode = 0;
    this->sramEnabled = false;
    clearRom(ROMSIZE);
    this->romLength = ROMSIZE;
    b = 0;
    c = 0;
    d = 0;
    e = 0;
    h = 0;
    l = 0;
    a = 0;
    f = 0;
    sp = 0;
    pc = 0;
}

memory::~memory() {
    assert(this->rom, "Something prematurely deleted the rom, is the destructor being called twice???");
    assert(this->wram, "Something prematurely deleted wram, is the destructor being called twice???");
    assert(this->vram, "Something prematurely deleted vram, is the destructor being called twice???");
    assert(this->hram, "Something prematurely deleted hram, is the destructor being called twice???");
    assert(this->oam, "Something prematurely deleted oam, is the destructor being called twice???");
    delete[](this->rom);
    delete[](this->wram);
    delete[](this->vram);
    delete[](this->hram);
    delete[](this->oam);
    if (this->m_bootRom) delete[](this->m_bootRom); // this actually *can* be deleted before here.
    rom = nullptr;
    wram = nullptr;
    vram = nullptr;
    oam = nullptr;
    m_bootRom = nullptr;
}

uint8_t memory::getReg8(const uint8_t number) const {
    switch (number) {
    case 0: return b;
    case 1: return c;
    case 2: return d;
    case 3: return e;
    case 4: return h;
    case 5: return l;
    case 6: throw std::logic_error("memory::getReg8 - invalid arg \"number\" cannot equal 6");
    case 7: return a;
    default:
        assert(0);
        throw std::logic_error("memory::getReg8 - invalid arg \"number\" cannot be greater than 7");
    }
}

void memory::setReg8(const uint8_t number, const uint8_t value) {
    switch (number)
    {
    case 0: b = value; break;
    case 1: c = value; break;
    case 2: d = value; break;
    case 3: e = value; break;
    case 4: h = value; break;
    case 5: l = value; break;
    case 6: throw std::logic_error("memory::setReg8 - invalid arg \"number\" cannot equal 6");
    case 7: a = value; break;
    default:
        assert(0);
        throw std::logic_error("memory::setReg8 - invalid arg \"number\" cannot be greater than 7");
    }
}

void memory::clearRom(const size_t size) noexcept {
    for (size_t i = 0; i < size; i++) this->rom[i] = 0xFF;
}

uint8_t memory::getMappedMemory(const uint16_t address) const {
    return getMappedMemory(address, false);
}

uint8_t memory::getMappedMemory(const uint16_t addressIn, const bool OAMDMARead) const {
    uint16_t address = addressIn;
    if (!OAMDMARead && false) // TODO: this is where I hook in the OAM DMA.
    {
        if (addressIn >= 0xFE00 && addressIn < 0xFEA0) return 0xFF;
        if (hasBusConflict(addressIn, 0xFE00)) address = 0xFE00; // TODO: hook in OAM DMA address here.
    }

    if (address < 0x4000) return getRomMemory(0, address); // ROM
    if (address < 0x8000) return getRomMemory(rombank, address); // ROM
    if (address < 0xA000) return this->vram[address - 0x8000]; // VRAM
    if (address < 0xC000) { // SRAM
        if (this->sramEnabled) UNIMPLEMENTEDREAD("SRAM", address - 0xA000);
        else return 0xFF;
    }
    if (address < 0xE000) return this->wram[address - 0xC000]; // WRAM
    if (address < 0xFE00) return this->wram[address - 0xE000]; // WRAM mirror
    if (address < 0xFEA0) return oam[address - 0xFE00]; // OAM
    if (address < 0xFF00) return wram[address - 0xFEA0];
    if (address < 0xFF80) UNIMPLEMENTEDREAD("IOREG", address - 0xFF00); // I/O REG
    if (address < 0xFFFF) return hram[address - 0xFF80]; // HRAM
    return this->ie; // IE
}

void memory::setMappedMemory(const uint16_t address, const uint8_t value) {
    if (address < 0x8000)
    {
        if (m_mbcMode == 0)
        {
            return;
        }

        throw NotImplementedException("memory::setMappedMemory::address<0x8000");
    }
    else setMappedMemory_common(address, value);
}

void memory::setR16(const uint8_t number, const uint8_t lowByte, const uint8_t highByte) {
    switch (number & 3) {
    case 0:
        c = lowByte;
        b = highByte;
        return;
    case 1:
        e = lowByte;
        d = highByte;
        return;
    case 2:
        l = lowByte;
        h = highByte;
        return;
    case 3:
        sp = lowByte;
        sp |= highByte << 8;
        return;
    }
}

uint8_t memory::getRomMemory(const size_t bank, const  uint16_t address) const {
    if (m_bootRom && address < 0x100 && bank == 0)
    {
        return m_bootRom[address];
    }
    if (m_mbcMode == 0) {
        if ((bank * ROMBANKSIZE) + address >= romLength) return 0xFF; // TODO: log?
        else return rom[address];
    }
    throw NotImplementedException("Unimplemented MBC mode");
}

void memory::setMappedMemory_common(const uint16_t address, const uint8_t value) {
    assert(address >= 0x8000);

    if (address < 0xA000) this->vram[address - 0x8000] = value; // VRAM
    else if (address < 0xC000) { if (this->sramEnabled) UNIMPLEMENTEDWRITE("SRAM", address - 0xA000); } // SRAM
    else if (address < 0xE000) this->wram[address - 0xC000] = value; // WRAM
    else if (address < 0xFE00) this->wram[address - 0xE000] = value; // WRAM mirror
    else if (address < 0xFEA0) { oam[address - 0xFE00]; } // OAM
    else if (address < 0xFF00) wram[address - 0xFEA0] = value; // Unused
    else if (address < 0xFF80) { UNIMPLEMENTEDWRITE("IOREG", address - 0xFF00); } // I/O REG
    else if (address < 0xFFFF) hram[address - 0xFF80] = value; // HRAM
    else ie = value; // IE
}
