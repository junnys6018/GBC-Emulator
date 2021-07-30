#include "gbc.h"

namespace gbc
{
    GBC::GBC(const std::string& file) : m_cartridge(Cartridge::from_rom(read_file(file))), m_bus(m_cartridge.get()), m_cpu(&m_bus), m_div(8)
    {
    }

    void GBC::step()
    {
        IORegisters* reg = &m_bus.m_registers;
        u32 cycles = m_cpu.step() * 4; // Multiply by 4 to convert from m-cycles to t-cycles
        reg->m_divider_register += m_div.add_cycles(cycles);
        if (reg->m_timer_control & GBC_TIMER_ENABLE_MASK)
        {
            u32 add = reg->m_timer.add_cycles(cycles);
            for (u32 i = 0; i < add; i++) // TODO: this can be optimised
            {
                bool overflow = (reg->m_timer_counter == 0xFF);
                if (overflow)
                {
                    reg->m_timer_counter = reg->m_timer_modulo;
                    reg->m_interrupt_flag |= GBC_INT_TIMER_MASK;
                }
                else
                {
                    reg->m_timer_counter++;
                }
            }
        }
    }
}