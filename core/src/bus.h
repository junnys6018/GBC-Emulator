#pragma once
#include "cartridge.h"
#include "io_registers.h"
#include "timer.h"
#include "ppu.h"
#include "util/common.h"

namespace gbc
{

    class Bus
    {
    public:
        Bus(GBC* gbc);
        u8 cpu_read_byte(u16 addr);
        void cpu_write_byte(u16 addr, u8 byte);
        u8 ppu_read_byte(u16 addr);
        u8 peek_byte(u16 addr) const;
        inline const u8* get_vram() const { return m_vram.data(); }
        inline const u8* get_wram() const { return m_wram.data(); }
        inline const u8* get_hram() const { return m_hram.data(); }
        inline const u8* get_oam() const { return m_oam.data(); }

        friend class GBC;
        friend class CPU;
        friend class PPU;
        friend class Timer;

    private:
        GBC* m_gbc;
        IORegisters m_registers;
        std::array<u8, 8192> m_vram;
        std::array<u8, 32768> m_wram;
        std::array<u8, 127> m_hram;
        std::array<u8, 160> m_oam;
    };
}