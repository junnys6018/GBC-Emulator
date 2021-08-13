#pragma once
#include "util/common.h"
#include "io_registers.h"

namespace gbc
{
    class GBC;
    class Timer
    {
    public:
        Timer(GBC* gbc);
        void clock(u32 t_cycles);
        u32 next_event();
        inline u8 read_divider() const { return m_system_clock >> 8; }
        void write_divider();
        void on_tac_write(u8 old_tac, u8 new_tac);

        friend class IORegisters;
    private:
        GBC* m_gbc;
        u16 m_system_clock = 0;
        u16 m_tma_reload = 0;
        u16 m_tma_blocked = 0;

        void increment_tima();
    };
}