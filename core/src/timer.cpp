#include "timer.h"
#include "gbc.h"

namespace gbc
{
    static u16 period_lut[4] = {10, 4, 6, 8};
    Timer::Timer(GBC* gbc) : m_gbc(gbc) {}

    void Timer::clock(u32 t_cycles)
    {
        IORegisters* reg = &m_gbc->m_bus.m_registers;

        u16 old_system_clock = m_system_clock;
        m_system_clock += t_cycles;

        if (m_tma_reload)
        {
            if (m_tma_reload <= t_cycles)
            {
                m_tma_reload = 0;
                reg->m_timer_counter = reg->m_timer_modulo;
                reg->m_interrupt_flag |= GBC_INT_TIMER_MASK;
            }
            else
            {
                m_tma_reload -= t_cycles;
            }
        }

        if (m_tma_blocked <= t_cycles)
            m_tma_blocked = 0;
        else
            m_tma_blocked -= t_cycles;

        if (reg->m_timer_control & GBC_TIMER_ENABLE_MASK)
        {
            u16 shift = period_lut[reg->m_timer_control & 3];
            u8 tima_add = (m_system_clock >> shift) - (old_system_clock >> shift);
            for (u32 i = 0; i < tima_add; i++)
            {
                increment_tima();
            }
        }
    }

    u32 Timer::next_event()
    {
        IORegisters* reg = &m_gbc->m_bus.m_registers;
        if (reg->m_timer_control & GBC_TIMER_ENABLE_MASK)
        {
            u32 clocks_to_overflow = 0x100 - (u32)reg->m_timer_counter;
            clocks_to_overflow--;
            u32 t_clocks_per_timer_clock = (1 << period_lut[reg->m_timer_control & 3]);
            u32 t_clocks_until_timer = t_clocks_per_timer_clock - (m_system_clock & (t_clocks_per_timer_clock - 1));
            u32 t_clocks_until_interupt = t_clocks_until_timer + clocks_to_overflow * t_clocks_per_timer_clock;
            return t_clocks_until_interupt + 4; // Add 4 cycles for reload delay
        }
        else
            return 0;
    }
    void Timer::write_divider()
    {
        IORegisters* reg = &m_gbc->m_bus.m_registers;
        if (reg->m_timer_control & GBC_TIMER_ENABLE_MASK)
        {
            u16 bit = period_lut[reg->m_timer_control & 3] - 1;
            if (m_system_clock & (1 << bit))
            {
                increment_tima();
            }
        }
        m_system_clock = 0;
    }

    void Timer::increment_tima()
    {
        IORegisters* reg = &m_gbc->m_bus.m_registers;
        bool overflow = (reg->m_timer_counter == 0xFF);
        if (overflow)
        {
            if (m_tma_reload)
            {
                // If we increment tima while its being reloaded, something very likely went wrong
                // we most likley emulated for too long before catch up, in any case, for correctness
                // we reload tima and increment. But emit a warning
                CORE_LOG_WARN("tima increment while reloading");
                m_tma_reload = 0;
                reg->m_timer_counter = reg->m_timer_modulo;
                reg->m_interrupt_flag |= GBC_INT_TIMER_MASK;
                increment_tima();
            }
            else
            {
                // set tima to 0 for 4 cycles
                reg->m_timer_counter = 0;
                m_tma_reload = 4;
                m_tma_blocked = 8;
            }
        }
        else
        {
            reg->m_timer_counter++;
        }
    }

    void Timer::on_tac_write(u8 olc_tac, u8 new_tac)
    {
        u16 old_bit = period_lut[olc_tac & 3] - 1;
        bool old_enable = ((olc_tac & GBC_TIMER_ENABLE_MASK) != 0);
        bool old_value = old_enable && ((m_system_clock & (1 << old_bit)) != 0);

        u16 new_bit = period_lut[new_tac & 3] - 1;
        bool new_enable = ((new_tac & GBC_TIMER_ENABLE_MASK) != 0);
        bool new_value = new_enable && ((m_system_clock & (1 << new_bit)) != 0);

        if (!new_value && old_value) // falling edge
            increment_tima();
    }
}
