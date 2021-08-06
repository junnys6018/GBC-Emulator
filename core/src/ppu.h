#pragma once
#include "util/common.h"

namespace gbc
{
    class GBC;

    class PPU
    {
    public:
        PPU(GBC* gbc);
        void clock();
        void run_until(u64 t_cycle);
        u32 next_event();
        inline const u32* get_framebuffer() const { return &m_framebuffer[m_frontbuffer][0]; }

    private:
        enum class PPUMode
        {
            HBLANK = 0,
            VBLANK = 1,
            OBJ = 2,
            DRAWING = 3,
        };
        GBC* m_gbc;
        u32 m_dot = 0;
        PPUMode m_mode = PPUMode::OBJ;
        u32 m_framebuffer[2][160 * 144];
        u32 m_frontbuffer = 0;
        bool m_stat_line = 0;
        u32 m_stall = 0;
        std::array<u8, 40> m_sprites;
    };

}
