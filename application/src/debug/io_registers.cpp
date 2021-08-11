#include "debug/io_registers.h"
#include <imgui.h>

namespace app
{
    void IORegistersWindow::draw_window(const char* title, const GBC& gbc)
    {
        ImGui::Begin(title);
        const IORegisters& reg = gbc.get_io_reg();
        ImGui::Text("[%.4X] DIV: %.2X", GBC_IOREG_DIVIDER_REGISTER, gbc.m_timer.read_divider());
        ImGui::Text("[%.4X] TIMA: %.2X", GBC_IOREG_TIMER_COUNTER, reg.m_timer_counter);
        ImGui::Text("[%.4X] TMA: %.2X", GBC_IOREG_TIMER_MODULO, reg.m_timer_modulo);
        ImGui::Text("[%.4X] TAC: %.2X", GBC_IOREG_TIMER_CONTROL, reg.m_timer_control);
        ImGui::Text("[%.4X] IF: %.2X", GBC_IOREG_INTERRUPT_FLAG, reg.m_interrupt_flag);
        ImGui::Text("[%.4X] IE: %.2X", 0xFFFF, reg.m_interrupt_enable);

        ImGui::Text("[%.4X] LCDC: %.2X", GBC_IOREG_LCDC, reg.m_lcdc);
        ImGui::Text("[%.4X] STAT: %.2X", GBC_IOREG_LCD_STAT, reg.m_lcd_stat);
        ImGui::Text("[%.4X] LYC: %.2X", GBC_IOREG_PPU_LYC, reg.m_ppu_lyc);
        ImGui::Text("[%.4X] SCY: %.2X", GBC_IOREG_PPU_SCY, reg.m_ppu_scy);
        ImGui::Text("[%.4X] SCX: %.2X", GBC_IOREG_PPU_SCX, reg.m_ppu_scx);
        ImGui::End();
    }
}