#include "io_registers.h"
#include "gbc.h"
#include "util/log.h"

namespace gbc
{
    IORegisters::IORegisters(GBC* gbc) : m_timer(10), m_gbc(gbc) {}
    u8 IORegisters::cpu_read_byte(u16 addr)
    {
        switch (addr)
        {
        case GBC_IOREG_JOYP: return m_joyp;
        case GBC_IOREG_DIVIDER_REGISTER: return m_divider_register;
        case GBC_IOREG_TIMER_COUNTER: return m_timer_counter;
        case GBC_IOREG_TIMER_MODULO: return m_timer_modulo;
        case GBC_IOREG_NR51: return m_nr51;
        case GBC_IOREG_TIMER_CONTROL: return m_timer_control;
        case GBC_IOREG_INTERRUPT_FLAG: return m_interrupt_flag;
        case GBC_IOREG_LCDC: return m_lcdc;
        case GBC_IOREG_LCD_STAT: return m_lcd_stat;
        case GBC_IOREG_PPU_SCY: return m_ppu_scy;
        case GBC_IOREG_PPU_SCX: return m_ppu_scx;
        case GBC_IOREG_PPU_LY: return m_ppu_ly;
        case GBC_IOREG_PPU_LYC: return m_ppu_lyc;
        case GBC_IOREG_DMA: return m_dma;
        case GBC_IOREG_PPU_BGP: return m_ppu_bgp;
        case GBC_IOREG_PPU_OBP0: return m_ppu_obp0;
        case GBC_IOREG_PPU_OBP1: return m_ppu_obp1;
        case GBC_IOREG_PPU_WY: return m_ppu_wy;
        case GBC_IOREG_PPU_WX: return m_ppu_wx;
        default: LOG_WARN("read to unimplemented io register {:4X}", addr); return 0;
        }
    }
    void IORegisters::cpu_write_byte(u16 addr, u8 byte)
    {
        static u32 period_lut[4] = {10, 4, 6, 8};
        switch (addr)
        {
        case GBC_IOREG_JOYP:
        {
            m_joyp = (byte & 0xF0);
            u8 keys = gbc::bit_cast<u8>(m_gbc->m_keys);
            if (!(m_joyp & GBC_ACTION_BTN_MASK))
                m_joyp |= (keys >> 4);
            if (!(m_joyp & GBC_DIRECTION_BTN_MASK))
                m_joyp |= (keys & 0x0F);
            return;
        }
        case GBC_IOREG_DIVIDER_REGISTER: m_divider_register = 0; return;
        case GBC_IOREG_TIMER_COUNTER: m_timer_counter = byte; return;
        case GBC_IOREG_TIMER_MODULO: m_timer_modulo = byte; return;
        case GBC_IOREG_NR51: m_nr51 = byte; return;
        case GBC_IOREG_TIMER_CONTROL:
            m_timer_control = byte;
            m_timer.m_period_log2 = period_lut[m_timer_control & GBC_TIMER_CLOCK_MASK];
            return;
        case GBC_IOREG_INTERRUPT_FLAG: m_interrupt_flag = byte; return;
        case GBC_IOREG_LCDC: m_lcdc = byte; return;
        case GBC_IOREG_LCD_STAT: m_lcd_stat = (byte & 0b01111000); return;
        case GBC_IOREG_PPU_SCY: m_ppu_scy = byte; return;
        case GBC_IOREG_PPU_SCX: m_ppu_scx = byte; return;
        case GBC_IOREG_PPU_LY: return; // write only
        case GBC_IOREG_PPU_LYC: m_ppu_lyc = byte; return;
        case GBC_IOREG_DMA:
            m_dma = byte;
            LOG_TRACE("oam dma started (page={:2X})", byte);
            if (byte >= 0xE0)
                LOG_ERROR("illegal value {:2X} written to 0xFF46 (dma)");
            m_gbc->m_oam_dma_transfer = 160;
            return;
        case GBC_IOREG_PPU_BGP: m_ppu_bgp = byte; return;
        case GBC_IOREG_PPU_OBP0: m_ppu_obp0 = byte; return;
        case GBC_IOREG_PPU_OBP1: m_ppu_obp1 = byte; return;
        case GBC_IOREG_PPU_WY: m_ppu_wy = byte; return;
        case GBC_IOREG_PPU_WX: m_ppu_wx = byte; return;
        default: LOG_WARN("write to unimplemented io register {:4X} {:2X}", addr, byte); return;
        }
    }
    u8 IORegisters::peek_byte(u16 addr) const
    {
        switch (addr)
        {
        }
        return const_cast<IORegisters*>(this)->cpu_read_byte(addr);
    }
}