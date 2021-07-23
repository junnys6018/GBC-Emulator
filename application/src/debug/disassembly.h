#pragma once
#include "common.h"
#include <gbc.h>

namespace app
{
    using namespace gbc;
    class DisassemblyWindow
    {
    public:
        DisassemblyWindow();

        void draw_window(const char* title, const GBC& gbc, u16 pc);
        void reset();

    private:
        using Line = std::array<char, 64>;

        // Settings
        u32 m_num_lines = 16;
        u32 m_offset = 8;

        std::vector<u16> m_base_addrs;
        std::vector<Line> m_lines;

    private:
        u16 disassemble(Line& line, const GBC& gbc, u16 pc);
        void fill_lines(const GBC& gbc, u16 pc);
        static inline bool is_empty(const Line& line) { return line[0] == '\0'; }
    };
}