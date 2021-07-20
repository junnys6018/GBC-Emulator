#pragma once
#include "util/common.h"

namespace gbc
{
    class Bus
    {
    public:
        Bus();
        u8 cpu_read_byte(u16 addr);
        void cpu_write_byte(u16 addr, u8 byte);

    private:
        u8 m_memory[65536]; // 64 kB
    };
}