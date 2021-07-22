#include "bus.h"

namespace gbc
{
    Bus::Bus() : m_memory()
    {
        // ld A, #00
        m_memory[0] = 0x3E;
        m_memory[1] = 0x00;

        // ld H, #01
        m_memory[2] = 0x26;
        m_memory[3] = 0x01;

        // ld L, #00
        m_memory[4] = 0x2E;
        m_memory[5] = 0x00;

        // ld (HL+), A
        m_memory[6] = 0x22;

        // inc A
        m_memory[7] = 0x3C;

        // jr #F3
        m_memory[8] = 0x18;
        m_memory[9] = 0xFC;
    }

    u8 Bus::cpu_read_byte(u16 addr)
    {
        return m_memory[addr];
    }

    void Bus::cpu_write_byte(u16 addr, u8 byte)
    {
        m_memory[addr] = byte;
    }
    u8* Bus::get_memory()
    {
        return m_memory.data();
    }
}