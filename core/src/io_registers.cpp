#include "io_registers.h"
#include "util/log.h"
#define LOG_WARN(...) (void)0

namespace gbc
{
    IORegisters::IORegisters() : m_timer(10) {}
    u8 IORegisters::cpu_read_byte(u16 addr)
    {
        switch (addr)
        {
        case GBC_IOREG_DIVIDER_REGISTER: return m_divider_register;
        case GBC_IOREG_TIMER_COUNTER: return m_timer_counter;
        case GBC_IOREG_TIMER_MODULO: return m_timer_modulo;
        case GBC_IOREG_TIMER_CONTROL: return m_timer_control;
        case GBC_IOREG_INTERRUPT_FLAG: return m_interrupt_flag;
        case GBC_IOREG_LCDC: return m_lcdc;
        default: LOG_WARN("read to unimplemented io register {}", addr); return 0;
        }
    }
    void IORegisters::cpu_write_byte(u16 addr, u8 byte)
    {
        static u32 period_lut[4] = {10, 4, 6, 8};
        switch (addr)
        {
        case GBC_IOREG_DIVIDER_REGISTER: m_divider_register = 0; return;
        case GBC_IOREG_TIMER_COUNTER: m_timer_counter = byte; return;
        case GBC_IOREG_TIMER_MODULO: m_timer_modulo = byte; return;
        case GBC_IOREG_TIMER_CONTROL:
            m_timer_control = byte;
            m_timer.m_period_log2 = period_lut[m_timer_control & GBC_TIMER_CLOCK_MASK];
            return;
        case GBC_IOREG_INTERRUPT_FLAG: m_interrupt_flag = byte; return;
        case GBC_IOREG_LCDC: m_lcdc = byte; return;
        default: LOG_WARN("write to unimplemented io register {}", addr); return;
        }
    }
    u8 IORegisters::peek_byte(u16 addr) const
    {
        switch (addr)
        {
        }
        return const_cast<IORegisters*>(this)->cpu_read_byte(addr);
    }
}