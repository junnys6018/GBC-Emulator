#pragma once
#include "divider.h"
#include "util/common.h"

//#define GBC_BG_TO_OAM_PRIORITY_MASK 0x80
//#define GBC_VERTICAL_FLIP_MASK 0x40
//#define GBC_HORIZONTAL_FLIP_MASK 0x20
//#define GBC_TILE_VRAM_BANK_MASK 0x08
//#define GBC_BG_PALETTE_MASK 0x07

// joyp masks
#define GBC_ACTION_BTN_MASK 0x20
#define GBC_DIRECTION_BTN_MASK 0x10
#define GBC_IOREG_JOYP 0xFF00

#define GBC_IOREG_DIVIDER_REGISTER 0xFF04
#define GBC_IOREG_TIMER_COUNTER 0xFF05
#define GBC_IOREG_TIMER_MODULO 0xFF06

// timer_control masks
#define GBC_TIMER_ENABLE_MASK 0x04
#define GBC_TIMER_CLOCK_MASK 0x03
#define GBC_IOREG_TIMER_CONTROL 0xFF07

// interrupt_flag masks
#define GBC_INT_JOYPAD_MASK 0x10
#define GBC_INT_SERIAL_MASK 0x08
#define GBC_INT_TIMER_MASK 0x04
#define GBC_INT_LCD_STAT_MASK 0x02
#define GBC_INT_VBLANK_MASK 0x01
#define GBC_IOREG_INTERRUPT_FLAG 0xFF0F

// lcdc masks
#define GBC_LCD_PPU_ENABLE_MASK 0x80
#define GBC_WINDOW_TILEMAP_AREA_MASK 0x40
#define GBC_WINDOW_ENABLE_MASK 0x20
#define GBC_BG_WINDOW_TILE_DATA_AREA_MASK 0x10
#define GBC_BG_TILEMAP_AREA_MASK 0x08
#define GBC_OBJ_SIZE_MASK 0x04
#define GBC_OBJ_ENABLE_MASK 0x02
#define GBC_BG_WINDOW_ENABLE_MASK 0x01
#define GBC_IOREG_LCDC 0xFF40

// lcd stat masks
#define GBC_INT_LYC_SRC_MASK 0x40
#define GBC_INT_OAM_SRC_MASK 0x20
#define GBC_INT_VBLANK_SRC_MASK 0x10
#define GBC_INT_HBLANK_SRC_MASK 0x08
#define GBC_LYC_FLAG_MASK 0x04
#define GBC_PPU_MODE_MASK 0x03
#define GBC_IOREG_LCD_STAT 0xFF41

#define GBC_IOREG_PPU_SCY 0xFF42
#define GBC_IOREG_PPU_SCX 0xFF43
#define GBC_IOREG_PPU_LY 0xFF44
#define GBC_IOREG_PPU_LYC 0xFF45
#define GBC_IOREG_PPU_BGP 0xFF47
#define GBC_IOREG_PPU_OBP0 0xFF48
#define GBC_IOREG_PPU_OBP1 0xFF49
#define GBC_IOREG_PPU_WY 0xFF4A
#define GBC_IOREG_PPU_WX 0xFF4B

namespace gbc
{
    class GBC;
    class IORegisters
    {
    public:
        IORegisters(GBC* gbc);
        u8 cpu_read_byte(u16 addr);
        void cpu_write_byte(u16 addr, u8 byte);
        u8 peek_byte(u16 addr) const;

    public:
        Divider m_timer;

        u8 m_joyp = 0xCF;             // 0xFF00
        u8 m_divider_register = 0x00; // 0xFF04
        u8 m_timer_counter = 0x00;    // 0xFF05
        u8 m_timer_modulo = 0x00;     // 0xFF06
        u8 m_timer_control = 0xF8;    // 0xFF07
        u8 m_interrupt_flag = 0xE1;   // 0xFF0F
        u8 m_lcdc = 0x91;             // 0xFF40
        u8 m_lcd_stat = 0x85;         // 0xFF41
        u8 m_ppu_scy = 0x00;          // 0xFF42
        u8 m_ppu_scx = 0x00;          // 0xFF43
        u8 m_ppu_ly = 0x00;           // 0xFF44
        u8 m_ppu_lyc = 0x00;          // 0xFF45
        u8 m_ppu_bgp = 0xFC;          // 0xFF47
        u8 m_ppu_obp0 = 0x00;         // 0xFF48
        u8 m_ppu_obp1 = 0x00;         // 0xFF49
        u8 m_ppu_wy = 0x00;           // 0xFF4A
        u8 m_ppu_wx = 0x00;           // 0xFF4B
        u8 m_interrupt_enable = 0x00; // 0xFFFF
    private:
        GBC* m_gbc;
    };
}