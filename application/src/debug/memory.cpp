#include "debug/memory.h"

namespace app
{
    static void set_options(MemoryEditor& mem_edit)
    {
        mem_edit.ReadOnly = true;
        mem_edit.OptShowOptions = false;
        mem_edit.OptShowAscii = true;
    }

    MemoryWindow::MemoryWindow()
    {
        set_options(m_rom_view);
        set_options(m_wram_view);
        set_options(m_hram_view);
    }

    void MemoryWindow::draw_window(const char* title, const GBC& gbc)
    {
        ImGui::Begin(title);

        if (ImGui::BeginTabBar("Memory Tabs"))
        {
            if (ImGui::BeginTabItem("rom")) // todo
            {
                m_rom_view.DrawContents((void*)gbc.get_rom(), 0x8000);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("wram"))
            {
                m_wram_view.DrawContents((void*)gbc.get_wram(), 0x2000, 0xC000);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("hram"))
            {
                m_hram_view.DrawContents((void*)gbc.get_hram(), 127, 0xFF80);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}