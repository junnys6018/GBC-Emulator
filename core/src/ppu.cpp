#include "ppu.h"
#include "bus.h"
#include "gbc.h"
#include "io_registers.h"

#define COL32(R, G, B, A) (((u32)(A) << 24) | ((u32)(B) << 16) | ((u32)(G) << 8) | ((u32)(R) << 0))

namespace gbc
{
    PPU::PPU(GBC* gbc) : m_gbc(gbc) { std::memset(m_framebuffer, 0, sizeof(m_framebuffer)); }

    void PPU::clock()
    {
        static u32 palette[4] = {COL32(155, 188, 15, 255), COL32(139, 172, 15, 255), COL32(48, 98, 48, 255), COL32(15, 56, 15, 255)};
        IORegisters* reg = &m_gbc->m_bus.m_registers;

        if (reg->m_lcdc & GBC_LCD_PPU_ENABLE_MASK)
        {
            const u8* vram = m_gbc->get_vram();

            if (m_stall == 0)
            {
                switch (m_mode)
                {
                case PPUMode::OBJ: m_stall = 80; break;
                case PPUMode::DRAWING:
                {
                    for (u8 fetcher_x = 0; fetcher_x < 20; fetcher_x++)
                    {
                        // Get tile
                        u32 tile_x_coarse = ((reg->m_ppu_scx >> 3) + fetcher_x) & 0x1F;
                        u32 tile_y_fine = (reg->m_ppu_scy + reg->m_ppu_ly) & 0xFF;
                        u32 tile_index = 32 * (tile_y_fine >> 3) + tile_x_coarse;
                        u32 tile_offset = (reg->m_lcdc & GBC_BG_TILEMAP_AREA_MASK) ? 0x1C00 : 0x1800;
                        u8 tile_id = vram[tile_offset + tile_index];

                        // Get tile data
                        u32 tile_addr;
                        if (reg->m_lcdc & GBC_BG_WINDOW_TILE_DATA_AREA_MASK)
                        {
                            tile_addr = 16 * tile_id + 2 * (tile_y_fine & 0x7);
                        }
                        else
                        {
                            tile_addr = 16 * bit_cast<i8>(tile_id) + 2 * (tile_y_fine & 0x7);
                        }
                        u8 tile_low = vram[tile_addr];
                        u8 tile_high = vram[tile_addr + 1];

                        for (i32 i = 0; i < 8; i++)
                        {
                            u8 mask = (0x80 >> i);
                            u8 pixel_id = ((tile_high & mask) ? 2 : 0) | ((tile_low & mask) ? 1 : 0);
                            u32 color = palette[pixel_id];
                            u32 framebuffer_index = reg->m_ppu_ly * 160 + fetcher_x * 8 + i;
                            m_framebuffer[1 - m_frontbuffer][framebuffer_index] = color;
                        }
                    }
                    m_mode = PPUMode::HBLANK;
                    m_stall = 160;

                    break;
                }
                case PPUMode::HBLANK: m_stall = 1; break;
                case PPUMode::VBLANK: m_stall = 1; break;
                }
            }
            m_stall--;
            m_dot++;
            if (reg->m_ppu_ly < 144)
            {
                if (m_dot == 80)
                    m_mode = PPUMode::DRAWING;
                else if (m_dot == 456)
                    m_mode = PPUMode::OBJ;
            }

            if (m_dot == 456)
            {
                m_dot = 0;
                reg->m_ppu_ly++;
                bool lyc = reg->m_ppu_ly == reg->m_ppu_lyc;
                reg->m_lcd_stat |= (lyc ? GBC_LYC_FLAG_MASK : 0);
                if (reg->m_ppu_ly == 144)
                {
                    m_mode = PPUMode::VBLANK;
                    reg->m_interrupt_flag |= GBC_INT_VBLANK_MASK;
                    m_frontbuffer = 1 - m_frontbuffer;
                }
                else if (reg->m_ppu_ly == 154)
                {
                    reg->m_ppu_ly = 0;
                    m_mode = PPUMode::OBJ;
                }
            }
            reg->m_lcd_stat = (reg->m_lcd_stat & ~GBC_PPU_MODE_MASK) | static_cast<u8>(m_mode);
            bool new_stat_line = ((reg->m_lcd_stat & GBC_LYC_FLAG_MASK) && (reg->m_lcd_stat & GBC_INT_LYC_SRC_MASK));
            new_stat_line = new_stat_line || ((m_mode == PPUMode::HBLANK) && (reg->m_lcd_stat & GBC_INT_HBLANK_SRC_MASK));
            new_stat_line = new_stat_line || ((m_mode == PPUMode::VBLANK) && (reg->m_lcd_stat & GBC_INT_VBLANK_SRC_MASK));
            new_stat_line = new_stat_line || ((m_mode == PPUMode::OBJ) && (reg->m_lcd_stat & GBC_INT_OAM_SRC_MASK));

            if (new_stat_line && !m_stat_line)
            {
                reg->m_interrupt_flag |= GBC_INT_LCD_STAT_MASK;
            }
            m_stat_line = new_stat_line;
        }
    }

    void PPU::run_until(u64 t_cycle) {}

}