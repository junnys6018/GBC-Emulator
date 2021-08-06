#include "ppu.h"
#include "bus.h"
#include "gbc.h"
#include "io_registers.h"

namespace gbc
{
    PPU::PPU(GBC* gbc) : m_gbc(gbc)
    {
        m_sprites.fill(0xFF);

        for (i32 i = 0; i < 160 * 144; i++)
            m_framebuffer[0][i] = COL32(155, 188, 15, 255);

        for (i32 i = 0; i < 160 * 144; i++)
            m_framebuffer[1][i] = COL32(155, 188, 15, 255);
    }

    void PPU::clock()
    {
        IORegisters* reg = &m_gbc->m_bus.m_registers;

        if (reg->m_lcdc & GBC_LCD_PPU_ENABLE_MASK)
        {
            Bus* bus = &m_gbc->m_bus;

            if (m_stall == 0)
            {
                switch (m_mode)
                {
                case PPUMode::OBJ:
                {
                    i32 sprite_buffer_index = 0;
                    for (i32 obj_addr = 0xFE00; obj_addr < 0xFEA0 && sprite_buffer_index < m_sprites.size(); obj_addr += 4)
                    {
                        i32 y_position = bus->ppu_read_byte(obj_addr);
                        i32 y_max = (reg->m_lcdc & GBC_OBJ_SIZE_MASK) ? y_position : y_position - 8;
                        if (reg->m_ppu_ly < y_max && reg->m_ppu_ly >= (y_position - 16))
                        {
                            m_sprites[sprite_buffer_index + 0] = y_position;
                            m_sprites[sprite_buffer_index + 1] = bus->ppu_read_byte(obj_addr + 1);
                            m_sprites[sprite_buffer_index + 2] = bus->ppu_read_byte(obj_addr + 2);
                            m_sprites[sprite_buffer_index + 3] = bus->ppu_read_byte(obj_addr + 3);
                            sprite_buffer_index += 4;
                        }
                    }
                    m_stall = 80;
                    break;
                }
                case PPUMode::DRAWING:
                {
                    u32 tile_y_fine = (reg->m_ppu_scy + reg->m_ppu_ly) & 0xFF;
                    u32 tile_offset = (reg->m_lcdc & GBC_BG_TILEMAP_AREA_MASK) ? 0x9C00 : 0x9800;
                    std::array<u8, 168> pixel_fifo;
                    for (u8 fetcher_x = 0; fetcher_x < 21; fetcher_x++)
                    {
                        // Get tile
                        u32 tile_x_coarse = ((reg->m_ppu_scx >> 3) + fetcher_x) & 0x1F;
                        u32 tile_index = 32 * (tile_y_fine >> 3) + tile_x_coarse;
                        u8 tile_id = bus->ppu_read_byte(tile_offset + tile_index);

                        // Get tile data
                        u16 tile_addr;
                        if (reg->m_lcdc & GBC_BG_WINDOW_TILE_DATA_AREA_MASK)
                        {
                            tile_addr = 0x8000 + 16 * tile_id + 2 * (tile_y_fine & 0x7);
                        }
                        else
                        {
                            tile_addr = 0x9000 + 16 * bit_cast<i8>(tile_id) + 2 * (tile_y_fine & 0x7);
                        }
                        u8 tile_low = bus->ppu_read_byte(tile_addr);
                        u8 tile_high = bus->ppu_read_byte(tile_addr + 1);

                        for (i32 i = 0; i < 8; i++)
                        {
                            u8 mask = (0x80 >> i);
                            u8 pixel_id = ((tile_high & mask) ? 2 : 0) | ((tile_low & mask) ? 1 : 0);
                            u32 fifo_index = fetcher_x * 8 + i;
                            pixel_fifo[fifo_index] = pixel_id;
                        }
                    }

                    u32 fine_x = (reg->m_ppu_scx & 0x07);
                    for (i32 i = 0; i < 160; i++)
                    {
                        u32 color = s_dmg_palette[pixel_fifo[i + fine_x]];
                        m_framebuffer[1 - m_frontbuffer][reg->m_ppu_ly * 160 + i] = color;
                    }

                    for (i32 sprite_idx = 0; sprite_idx < m_sprites.size(); sprite_idx += 4)
                    {
                        i32 y_position = m_sprites[sprite_idx + 0];
                        i32 x_position = m_sprites[sprite_idx + 1];
                        i32 tile_id = m_sprites[sprite_idx + 2];
                        i32 attributes = m_sprites[sprite_idx + 3];

                        if (reg->m_lcdc & GBC_OBJ_SIZE_MASK)
                            tile_id = tile_id & 0xFE;

                        // Get tile data
                        u32 fine_y = reg->m_ppu_ly - (y_position - 16);
                        if (attributes & GBC_VERTICAL_FLIP_MASK)
                        {
                            fine_y = ((reg->m_lcdc & GBC_OBJ_SIZE_MASK) ? 15 : 7) - fine_y;
                        }

                        u16 tile_addr = 0x8000 + 16 * tile_id + 2 * fine_y;

                        u8 tile_low = bus->ppu_read_byte(tile_addr);
                        u8 tile_high = bus->ppu_read_byte(tile_addr + 1);

                        for (i32 i = 0; i < 8; i++)
                        {
                            i32 x_pos = x_position - 8 + i;
                            if (x_pos < 0 || x_pos >= 160)
                                continue;
                            u8 mask = ((attributes & GBC_HORIZONTAL_FLIP_MASK) ? (0x01 << i) : (0x80 >> i));

                            u8 pixel_id = ((tile_high & mask) ? 2 : 0) | ((tile_low & mask) ? 1 : 0);
                            if (pixel_id == 0) // transparent
                                continue;
                            u32 color = s_dmg_palette[pixel_id];
                            u32 framebuffer_index = reg->m_ppu_ly * 160 + x_pos;
                            m_framebuffer[1 - m_frontbuffer][framebuffer_index] = color;
                        }
                    }

                    m_mode = PPUMode::HBLANK;
                    m_stall += 160;
                    m_sprites.fill(0xFF);
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

                if (reg->m_ppu_ly == reg->m_ppu_lyc)
                    reg->m_lcd_stat |= GBC_LYC_FLAG_MASK;
                else
                    reg->m_lcd_stat &= ~GBC_LYC_FLAG_MASK;

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

            if (new_stat_line && !m_stat_line) // rising edge
            {
                reg->m_interrupt_flag |= GBC_INT_LCD_STAT_MASK;
            }
            m_stat_line = new_stat_line;
        }
    }

    void PPU::run_until(u64 t_cycle) {}

    u32 PPU::next_event() { return 0; }

}