#pragma once
#include "memory.h"

typedef long long cycle_t;
const cycle_t MCYCLE = 4;
const cycle_t TCYCLE = 1;
class cpu {
public:
    void f_tick(const cycle_t tcycles);
    cpu(uint8_t * const p_bootRom);

private:
    inline void f_ldu16r(const uint8_t r16Num);
    inline uint8_t f_readcycleU8();
    bool m_ime;
    bool m_nextIME;
    enum state { okay, halt, stop, hung };
    void f_updateDevices();
    void f_updateDevices(const cycle_t tcycles);
    void f_handleInterrupts();
    void f_runIstr();
    void f_runCbInstr();
    inline void f_push(const uint8_t rHigh, const uint8_t rLow);
    inline void f_pop(uint8_t & rHigh, uint8_t & rLow);
    inline void f_cbWrite(const uint8_t src, const uint8_t val);
    inline void f_jr8(const bool jump);
    inline void f_ret(const bool ei);
    inline bool f_condJump(const bool zero);
    inline void f_ldrD8(const uint8_t rNum);
    inline void f_inc8(const uint8_t rNum);
    inline void f_dec8(const uint8_t rNum);
    inline void f_inc16(const uint8_t r16Num);
    inline void f_dec16(const uint8_t r16Num);
    memory m_memory;
    bool m_failedHalt;
    inline void f_ldRR(const uint8_t src, const uint8_t dest);
    uint8_t f_readcycle(const uint16_t address);
    uint8_t f_readcyclePop();
    inline void f_call(bool call);
    void f_writecycle(const uint16_t address, const uint8_t value);
    inline uint8_t f_readcycleHL();
    inline void f_writecycleHL(const uint8_t value);
    inline void f_writecyclePush(const uint8_t value);
    cycle_t m_syncCycle;
    state m_state;
    inline uint8_t f_getarrithval(const uint8_t src);
    inline void cpu::f_ldR16a(const uint8_t r16Num);
    inline void cpu::f_ldaR16(const uint8_t r16Num);
};
