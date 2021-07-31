#include "debug/stack.h"
#include <imgui.h>

namespace app
{
    inline u16 min(u16 a, u16 b) { return (a < b) ? a : b; }
    inline i32 clamp(i32 val, i32 min, i32 max)
    {
        if (val < min)
            val = min;
        if (val >= max)
            val = max - 1;
        return val;
    }
    void StackWindow::draw_window(const char* title, const GBC& gbc)
    {
        ImGui::Begin(title);
        if (ImGui::InputInt("Lines", &m_lines))
        {
            m_lines = clamp(m_lines, 0, 0x10000);
        }

        u16 sp = gbc.get_sp();
        u16 num_lines = min(m_lines, 0xFFFF - sp + 1);
        u16 base = sp + num_lines - 1;
        for (i32 i = base; i >= sp; i--)
        {
            u8 byte = gbc.peek_byte(i);
            ImGui::Text("$%.4X: %.2X", i, byte);
        }

        ImGui::End();
    }

}
