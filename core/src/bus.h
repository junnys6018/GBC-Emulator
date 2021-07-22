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
        u8* get_memory();

    private:
        std::array<u8, 65536> m_memory; // 64 kB
    };
}