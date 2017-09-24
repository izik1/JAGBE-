#pragma once
#include <cstdint>
#include "memory.h"
#include "cycling.h"
class lcd
{
public:
    const int WIDTH = 160;
    const int HEIGHT = 144;
    uint8_t m_lcdc;
    inline uint8_t stat() { return m_mode | m_statupper; }
    uint8_t getReg(const uint8_t num) const;
    void setReg(const uint8_t num, const uint8_t value);

    /// <summary>
    /// This is owned by the lcd. don't delete unless you are the lcd.
    /// </summary>
    uint8_t* m_displayBuffer;

    /// <summary>
    /// Binds the specified vram and oam, this *must* be done before this object can be used.
    /// </summary>
    /// <param name="vram">The vram.</param>
    /// <param name="oam">The oam.</param>
    void bind(uint8_t* const vram, uint8_t* const oam);
    void unbind();
    void update(uint8_t & p_if);

    bool updateVblankswitch(uint8_t & p_if);

    lcd();
    ~lcd();
private:
    inline bool cpLy();
    uint8_t m_lyc;
    uint8_t m_mode;
    uint8_t m_statupper;
    bool updateNormalLine();
    bool m_pstatIRQ;
    uint8_t m_visibleLy;
    uint8_t m_ly;
    void disable();
    bool m_disabled;
    cycle_t cycleMod;

    /// <summary>
    /// This isn't owned by the lcd.
    /// </summary>
    uint8_t* m_vram;

    /// <summary>
    /// This isn't owned by the lcd.
    /// </summary>
    uint8_t* m_oam;
};
