#include "bus.h"

namespace gbc
{
    Bus::Bus()
    {
    }

    u8 Bus::cpu_read_byte(u16 addr)
    {
        return m_memory[addr];
    }

    void Bus::cpu_write_byte(u16 addr, u8 byte)
    {
        m_memory[addr] = byte;
    }
}