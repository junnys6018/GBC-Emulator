#include "debug/tilemap.h"
#include <imgui.h>
namespace app
{
    enum AddressingMode
    {
        AddressingMode8000,
        AddressingMode8800
    };

    TilemapWindow::TilemapWindow() : m_texture(256, 256, nullptr, GL_RGBA) {}

    static void render_tilemap(u32 bitmap[256 * 256], const u8* vram, AddressingMode mode)
    {
        static u32 palette[4] = {IM_COL32(155, 188, 15, 255), IM_COL32(139, 172, 15, 255), IM_COL32(48, 98, 48, 255),
                                 IM_COL32(15, 56, 15, 255)};

        for (u32 tile_x = 0; tile_x < 32; tile_x++)
        {
            for (u32 tile_y = 0; tile_y < 32; tile_y++)
            {
                u32 tilemap_index = tile_y * 32 + tile_x;
                u8 tile_id = vram[tilemap_index + 0x1800];
                u32 tiledata_addr;
                if (mode == AddressingMode8000)
                    tiledata_addr = 16 * tile_id;
                else if (mode == AddressingMode8800)
                    tiledata_addr = 0x8800 + 16 * gbc::bit_cast<i8>(tile_id);

                for (u32 line = 0; line < 8; line++)
                {
                    u8 lsb = vram[tiledata_addr++];
                    u8 msb = vram[tiledata_addr++];
                    for (u32 pixel = 0; pixel < 8; pixel++)
                    {
                        u8 mask = 0x80 >> pixel;
                        u8 color_id = ((lsb & mask) ? 1 : 0) | ((msb & mask) ? 2 : 0);

                        u32 pixel_x = tile_x * 8 + pixel;
                        u32 pixel_y = tile_y * 8 + line;
                        u32 bitmap_index = pixel_y * 256 + pixel_x;

                        bitmap[bitmap_index] = palette[color_id];
                    }
                }
            }
        }
    }
    void TilemapWindow::draw_window(const char* title, const GBC& gbc)
    {
        ImGui::Begin(title);
        u32 tilemap[256 * 256];
        const u8* vram = gbc.get_vram();
        AddressingMode mode = (gbc.get_io_reg().m_lcdc & GBC_BG_WINDOW_TILE_DATA_AREA_MASK) ? AddressingMode8000 : AddressingMode8800;
        render_tilemap(tilemap, vram, mode);
        m_texture.set_data(tilemap);

        ImGui::Image((void*)m_texture.m_handle, {256 * m_scale, 256 * m_scale});
        ImGui::End();
    }
}