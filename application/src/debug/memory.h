#pragma once
#include <gbc.h>
#include <imgui.h>
#include <imgui_memory_editor.h>

namespace app
{
    using namespace gbc;
    class MemoryWindow
    {
    public:
        MemoryWindow();
        void draw_window(const char* title, const GBC& gbc);

    private:
        MemoryEditor m_rom_view;
        MemoryEditor m_wram_view;
        MemoryEditor m_hram_view;
    };
}