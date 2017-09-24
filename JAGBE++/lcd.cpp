#include "lcd.h"
#include <cassert>
#include <algorithm>
#include "cycling.h"
#include "util.h"

inline bool lcd::cpLy() {
    if ((cycleMod != 0 && m_lyc == m_visibleLy) || (cycleMod == 0 && m_lyc == 0))
    {
        m_statupper |= bit(2);
        return m_statupper & bit(6);
    }
    else
    {
        m_statupper &= nbit(2);
        return false;
    }
}

uint8_t lcd::getReg(const uint8_t num) const {
    switch (num) {
    case 0x0: return m_lcdc;
    case 0x1: return bit(7) | m_statupper | m_mode;
    case 0x4: return m_visibleLy;
    default:
        return 0xFF;
    }
}

void lcd::setReg(const uint8_t num, const uint8_t value) {
    switch (num) {
    case 0x0: m_lcdc = value; break;
    case 0x1: m_statupper = (value & 0b01111000); break;
    }
}

void lcd::bind(uint8_t * const vram, uint8_t* const oam) {
    assert(vram);
    assert(oam);
    m_vram = vram;
    m_oam = oam;
}

void lcd::unbind() {
    m_vram = nullptr;
    m_oam = nullptr;
}

void lcd::update(uint8_t & p_if) {
    assert(m_vram);
    assert(m_oam); // Assert bound
    if (!(m_lcdc & bit(7))) return;
    else m_disabled = false;

    bool irq;
    if (m_ly < 144) irq = updateNormalLine();
    else if (m_ly == 144) irq = updateVblankswitch(p_if);
    else irq = cpLy() || m_statupper & (bit(4) | bit(5));

    cycleMod++;
    if (cycleMod == cycle::MCYCLE * 114) {
        cycleMod = 0;
        m_ly++;
    }

    if (cycleMod == 0 && m_ly > 0) m_visibleLy++;

    if (m_ly == 153 && cycleMod == cycle::MCYCLE) m_visibleLy = 0;
}

bool lcd::updateVblankswitch(uint8_t & p_if)
{
    if (cycleMod == 0)
    {
        m_mode = 0b00;
        return m_statupper & bit(3);
    }
    else
    {
        if (cycleMod == cycle::MCYCLE)
        {
            p_if |= bit(0);
            m_mode = 0b01;
        }

        return cpLy() || (m_statupper & (bit(4) | bit(5)));
    }
}

lcd::lcd() {
    m_lcdc = 0;
    m_lyc = 0;
    m_displayBuffer = new uint8_t[WIDTH*HEIGHT];
    std::fill(m_displayBuffer, m_displayBuffer + WIDTH*HEIGHT, 0);
    m_vram = nullptr;
    m_oam = nullptr;
    m_disabled = false;
    disable();
}

void lcd::disable() {
    if (m_disabled) return;
    m_disabled = true;
    cycleMod = 0;
    m_ly = 0;
    m_visibleLy = 0;
    m_pstatIRQ = false;
    std::fill(m_displayBuffer, m_displayBuffer + WIDTH*HEIGHT, 0);
}

lcd::~lcd() {
    assert(m_displayBuffer);
    delete[] m_displayBuffer;
}

bool lcd::updateNormalLine() {
    switch (cycleMod) {
    case cycle::MCYCLE * 0:
        m_mode = 0b00;
        break;

    case cycle::MCYCLE:
        m_mode = 0b10;
        break;

    case cycle::MCYCLE * 10:
        m_mode = 0b11;
        break;

    case cycle::MCYCLE * 53:
        m_mode = 0b00;
        break;
    }

    return cpLy() || (m_mode == 0b10 && (m_statupper & bit(5))) || (m_mode == 0b00 && (m_statupper & bit(3)));
}
