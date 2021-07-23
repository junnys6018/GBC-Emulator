#pragma once
#include "util/common.h"
#include "cartridge.h"

namespace gbc
{
    class Bus
    {
    public:
        Bus(Cartridge* cartridge);
        u8 cpu_read_byte(u16 addr);
        void cpu_write_byte(u16 addr, u8 byte);
        u8 peek_byte(u16 addr) const;

    private:
        Cartridge* m_cartridge;
        std::array<u8, 8192> m_vram;
        std::array<u8, 32768> m_wram;
    };
}