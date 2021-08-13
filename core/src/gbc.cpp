#include "gbc.h"

namespace gbc
{
    GBC::GBC(const std::string& file)
        : m_cartridge(Cartridge::from_rom(read_file(file))), m_bus(this), m_cpu(&m_bus), m_ppu(this), m_timer(this)
    {
    }

    void GBC::step()
    {
        IORegisters* reg = &m_bus.m_registers;
        if (!m_cpu.m_halted || (reg->m_interrupt_enable & reg->m_interrupt_flag & 0x1F))
        {
            m_cpu.m_halted = false;
            u32 cycles = m_cpu.step() * 4; // Multiply by 4 to convert from m-cycles to t-cycles
            m_total_t_cycles += cycles;
            m_timer.clock(cycles);

            if (m_oam_dma_transfer)
            {
                u32 count = std::min(cycles / 4, m_oam_dma_transfer);
                do_dma_cycles(count);
            }

            for (int i = 0; i < cycles; i++)
                m_ppu.clock();
        }
        else
        {
            u32 clocks = m_timer.next_event();
            clocks = clamped_min(clocks, m_ppu.next_event());
            m_total_t_cycles += clocks;

            m_timer.clock(clocks);

            if (m_oam_dma_transfer)
            {
                u32 count = std::min((u32)(clocks + m_total_t_cycles % 4) / 4, m_oam_dma_transfer);
                do_dma_cycles(count);
            }

            for (i32 i = 0; i < clocks; i++)
                m_ppu.clock();

            if (clocks == 0)
                CORE_LOG_INFO("GBC halted forever");
        }
    }

    void GBC::clock()
    {
        IORegisters* reg = &m_bus.m_registers;
        m_total_t_cycles++;

        if (m_oam_dma_transfer > 0 && (m_total_t_cycles % 4 == 0))
        {
            do_dma_cycles(1);
        }

        if (!m_cpu.m_halted || (reg->m_interrupt_enable & reg->m_interrupt_flag & 0x1F))
        {
            m_cpu.m_halted = false;

            if (m_total_t_cycles % 4 == 0)
                m_cpu.clock();
            m_timer.clock(1);
            m_ppu.clock();
        }
        else
        {
            m_timer.clock(1);
            m_ppu.clock();
        }
    }

    void GBC::set_keys(Keys keys) { m_keys = keys; }

    void GBC::do_dma_cycles(u32 count)
    {
        IORegisters* reg = &m_bus.m_registers;
        u16 oam_addr = 160 - m_oam_dma_transfer;
        u16 src_addr = (static_cast<u16>(reg->m_dma) << 8) + (160 - m_oam_dma_transfer);

        for (i32 i = 0; i < count; i++)
        {
            u8 byte = m_bus.cpu_read_byte(src_addr++);
            m_bus.m_oam[oam_addr++] = byte;
        }

        m_oam_dma_transfer -= count;
    }
}