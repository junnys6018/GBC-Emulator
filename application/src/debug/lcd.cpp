#include "debug/lcd.h"
#include <imgui.h>
namespace app
{
    LCDWindow::LCDWindow() : m_texture(160, 144, nullptr, GL_RGBA) {}

    void LCDWindow::draw_window(const char* title, const GBC& gbc)
    {
        ImGui::Begin(title);
        const u32* framebuffer = gbc.get_framebuffer();
        m_texture.set_data(framebuffer);

        ImGui::Image((void*)m_texture.m_handle, {160 * 2, 144 * 2});
        ImGui::End();
    }
}