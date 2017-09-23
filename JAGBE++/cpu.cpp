#include "cpu.h"
#include "flags.h"
#include "jagbe++exceptions.h"
#include <cassert>
#include <cstdint>
#include <iostream>
inline uint8_t cpu::f_getarrithval(const uint8_t src) {
    return src == 0b110 ? f_readcycleHL() : m_memory.getReg8(src);
}

inline void cpu::f_ldu16r(const uint8_t r16Num) {
    m_memory.setR16(r16Num, f_readcycleU8(), f_readcycleU8());
}

inline void cpu::f_ldaR16(const uint8_t r16Num) {
    switch (r16Num) {
    case 0:
        m_memory.a = f_readcycle((m_memory.b << 8) | m_memory.c);
        break;
    case 1:
        m_memory.a = f_readcycle((m_memory.d << 8) | m_memory.e);
        break;
    case 2:
        m_memory.a = f_readcycleHL();
        m_memory.hl(m_memory.hl() + 1);
        break;
    case 3:
        m_memory.a = f_readcycleHL();
        m_memory.hl(m_memory.hl() - 1);
        break;
    default:
        assert(0);
    }
}

inline void cpu::f_ldR16a(const uint8_t r16Num) {
    switch (r16Num) {
    case 0:
        f_writecycle((m_memory.b << 8) | m_memory.c, m_memory.a);
        break;
    case 1:
        f_writecycle((m_memory.d << 8) | m_memory.e, m_memory.a);
        break;
    case 2:
        f_writecycleHL(m_memory.a);
        m_memory.hl(m_memory.hl() + 1);
        break;
    case 3:
        f_writecycleHL(m_memory.a);
        m_memory.hl(m_memory.hl() - 1);
        break;
    default:
        assert(0);
    }
}

void cpu::f_runIstr() {
    f_updateDevices(2);
    f_handleInterrupts();
    f_updateDevices(1);
    uint8_t op = m_memory.getMappedMemory(m_memory.pc++);
    std::cerr << m_memory.pc << " " << int(op) << std::endl;
    f_updateDevices(1);

    switch (op & 0xC0) {
    case 0x00:
        switch (op) {
        case 0x01: case 0x11: case 0x21: case 0x31: // LD,r16,d16
            f_ldu16r(op >> 4); break;

        case 0x02: case 0x12: case 0x22: case 0x32: // LD,(r16),a
            f_ldR16a(op >> 4); break;

        case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: case 0x34: case 0x3C:
            f_inc8(op >> 3); break;

        case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: case 0x36: case 0x3E:
            f_ldrD8(op >> 3);  break;

        case 0x0A: case 0x1A: case 0x2A: case 0x3A: // LD,A,(r16)
            f_ldaR16(op >> 4); break;

        case 0x18: f_jr8(true); break;
        case 0x20: f_jr8(!f_condJump(true));  break;
        case 0x28: f_jr8(f_condJump(true));   break;
        case 0x30: f_jr8(!f_condJump(false)); break;
        case 0x38: f_jr8(f_condJump(false));  break;
        default: throw unimplementedInstructionException(op);
        }
        break;
    case 0x40: {
        if (op == 0x79) throw unimplementedInstructionException(op);
        else f_ldRR((op >> 3) & 0b111, op & 0b111);
        break;
    }
    case 0x80: {
        switch ((op >> 3) & 0b111) {
        case 0x00: { // ADD A,R
            uint8_t val = f_getarrithval(op & 0b111);
            uint8_t sum = val + m_memory.a;
            m_memory.f = flags::f_getZF8(sum) | flags::f_getHC8(m_memory.a, val) | flags::f_getCF8(m_memory.a, sum);
            break;
        }
        case 0x05: { // XOR,A,R
            m_memory.a = f_getarrithval(op & 0b111) ^ m_memory.a;
            m_memory.f = flags::f_getZF8(m_memory.a);
            break;
        }
        default:
            throw unimplementedInstructionException(op);
        }

        break;
    }
    case 0xC0:
        switch (op) {
        case 0xC4: f_call(!f_condJump(true)); break;
        case 0xCB: f_runCbInstr(); break;
        case 0xCC: f_call(f_condJump(true)); break;
        case 0xCD: f_call(true); break;
        case 0xD4: f_call(!f_condJump(false)); break;
        case 0xDC: f_call(f_condJump(false)); break;
        case 0xE0: f_writecycle(0xFF00 | f_readcycleU8(), m_memory.a); break;
        case 0xE2: f_writecycle(0xFF00 | m_memory.c, m_memory.a); break;
        case 0xF0: m_memory.a = f_readcycle(0xFF00 | f_readcycleU8()); break;
        case 0xF2: m_memory.a = f_readcycle(0xFF00 | m_memory.c); break;

        default: throw unimplementedInstructionException(op);
        }

        break;
    }
}

void cpu::f_runCbInstr() {
    uint8_t op = f_readcycleU8();
    uint8_t r = op & 0b111;
    uint8_t v = r == 0b110 ? f_readcycleHL() : m_memory.getReg8(r);
    uint8_t bit = (op >> 3) & 0b111;
    switch (op & 0xC0) {
    case 0x00:
        throw NotImplementedException();
    case 0x40: m_memory.f = flags::f_getZF8(v & (1 << bit)) |
        flags::HALFBIT | (m_memory.f & flags::CARRYBIT); break; // BIT b,r8;
    case 0x80: f_cbWrite(r, v & (~(1 << bit))); break; // RES b,r8
    case 0xC0: f_cbWrite(r, v | (1 << bit)); break; // SET b,r8
    }
}

inline void cpu::f_cbWrite(const uint8_t src, const uint8_t val) {
    if (src == 0b110) f_writecycleHL(val);
    else m_memory.setReg8(src, val);
}

inline void cpu::f_jr8(bool jump) {
    f_updateDevices(MCYCLE);

    if (!jump) m_memory.pc++;
    else m_memory.pc += int8_t(f_readcycleU8());
}

inline bool cpu::f_condJump(bool zero)
{
    return m_memory.f & (zero ? flags::ZEROBIT : flags::CARRYBIT);
}

inline void cpu::f_ldrD8(uint8_t rNum) { return f_cbWrite(rNum, f_readcycleU8()); }

inline void cpu::f_inc8(uint8_t rNum) {
    f_cbWrite(rNum, (rNum == 0b110 ? f_readcycleHL() : m_memory.getReg8(rNum) + 1));
}

inline void cpu::f_ldRR(const uint8_t dest, const uint8_t src) {
    if (dest == 0b110 || src == 0b110) {
        assert(src != dest);

        if (src == 0b110)
            m_memory.setReg8(dest, f_readcycle(m_memory.hl()));
        else
            this->f_writecycle(m_memory.hl(), (m_memory.getReg8(src)));
        return;
    }

    m_memory.setReg8(dest, m_memory.getReg8(src));
}

uint8_t cpu::f_readcycle(const uint16_t address) {
    f_updateDevices(4);
    return m_memory.getMappedMemory(address);
}

inline void cpu::f_call(bool call) {
    uint16_t addr = f_readcycleU8() | (f_readcycleU8() << 8);
    std::cerr << "call addr: " << addr << std::endl;
    if (!call)
    {
        return;
    }

    f_updateDevices(MCYCLE);
    f_writecyclePush(m_memory.pc >> 8);
    f_writecyclePush(m_memory.pc & 0xFF);
    m_memory.pc = addr;
}

void cpu::f_writecycle(const uint16_t address, const uint8_t value) {
    f_updateDevices(4);
    m_memory.setMappedMemory(address, value);
}

inline uint8_t cpu::f_readcycleHL() { return f_readcycle(m_memory.hl()); }
inline uint8_t cpu::f_readcycleU8() { return f_readcycle(m_memory.pc++); }

inline void cpu::f_writecycleHL(const uint8_t value) {
    f_writecycle(m_memory.hl(), value);
}

inline void cpu::f_writecyclePush(const uint8_t value) { f_writecycle(m_memory.sp--, value); }

void cpu::f_tick(const cycle_t tcycles) {
    m_syncCycle += tcycles;
    while (m_syncCycle > 0) {
        switch (m_state) {
        case cpu::okay:
            f_runIstr();
            break;
        case cpu::halt:
            throw NotImplementedException("cpu::f_tick::halt");
            break;
        case cpu::stop:
            throw NotImplementedException("cpu::f_tick::stop");
            break;
        case cpu::hung:
            throw NotImplementedException("cpu::f_tick::hung");
            break;
        }
    }
}

cpu::cpu(uint8_t * const p_bootRom) : m_memory(p_bootRom) {
    m_state = cpu::okay;
    m_syncCycle = 0;
    m_ime = false;
    m_failedHalt = false;
}

void cpu::f_updateDevices() {
    m_syncCycle--;
    return; // Nothing to update yet.
}

void cpu::f_updateDevices(const cycle_t tcycles) {
    for (cycle_t i = 0; i < tcycles; i++)
        f_updateDevices();
}

void cpu::f_handleInterrupts() {
    if (!m_ime || !(m_memory.ie & m_memory.m_if & 0x1F)) {
        return;
    }

    throw NotImplementedException("cpu::f_handleInterrupts");
}