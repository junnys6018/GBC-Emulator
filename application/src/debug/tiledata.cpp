#include "debug/tiledata.h"
#include <imgui.h>
namespace app
{
    TiledataWindow::TiledataWindow() : m_texture(128, 64 * 3, nullptr, GL_RGBA) {}
    static void render_tiledata(u32 bitmap[128 * 64], const u8* vram)
    {
        static u32 palette[4] = {IM_COL32(155, 188, 15, 255), IM_COL32(139, 172, 15, 255), IM_COL32(48, 98, 48, 255),
                                 IM_COL32(15, 56, 15, 255)};
        u32 i = 0;
        for (u32 tile = 0; tile < 128; tile++)
        {
            for (u32 line = 0; line < 8; line++)
            {
                u8 lsb = vram[i++];
                u8 msb = vram[i++];
                for (u32 pixel = 0; pixel < 8; pixel++)
                {
                    u8 mask = 0x80 >> pixel;
                    u8 color_id = ((lsb & mask) ? 1 : 0) | ((msb & mask) ? 2 : 0);

                    u32 tile_x = (tile & 0xF);
                    u32 tile_y = (tile >> 4);
                    u32 pixel_x = tile_x * 8 + pixel;
                    u32 pixel_y = tile_y * 8 + line;
                    u32 bitmap_index = pixel_y * 8 * 16 + pixel_x;

                    bitmap[bitmap_index] = palette[color_id];
                }
            }
        }
    }
    void TiledataWindow::draw_window(const char* title, const GBC& gbc)
    {
        ImGui::Begin(title);
        u32 block[128 * 64];
        const u8* vram = gbc.get_vram();

        ImGui::SliderFloat("Scale", &m_scale, 1, 4);

        render_tiledata(block, vram);
        m_texture.set_data(block, 0, 0, 128, 64);
        render_tiledata(block, vram + 0x800);
        m_texture.set_data(block, 0, 64, 128, 64);
        render_tiledata(block, vram + 0x1000);
        m_texture.set_data(block, 0, 128, 128, 64);

        ImGui::Image((void*)m_texture.m_handle, {128 * m_scale, 192 * m_scale});

        ImGui::End();
    }

}
