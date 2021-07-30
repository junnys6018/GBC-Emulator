#include "debug/io_registers.h"
#include <imgui.h>

namespace app
{
    void IORegistersWindow::draw_window(const char* title, const GBC& gbc)
    {
        ImGui::Begin(title);
        const IORegisters& reg = gbc.get_io_reg();
        ImGui::Text("[%.4X] DIV: %.2X", GBC_IOREG_DIVIDER_REGISTER, reg.m_divider_register);
        ImGui::Text("[%.4X] TIMA: %.2X", GBC_IOREG_TIMER_COUNTER, reg.m_timer_counter);
        ImGui::Text("[%.4X] TMA: %.2X", GBC_IOREG_TIMER_MODULO, reg.m_timer_modulo);
        ImGui::Text("[%.4X] TAC: %.2X", GBC_IOREG_TIMER_CONTROL, reg.m_timer_control);
        ImGui::Text("[%.4X] IF: %.2X", GBC_IOREG_INTERRUPT_FLAG, reg.m_interrupt_flag);
        ImGui::Text("[%.4X] IE: %.2X", 0xFFFF, reg.m_interrupt_enable);
        ImGui::End();
    }
}