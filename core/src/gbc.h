#pragma once
#include "bus.h"
#include "cpu.h"
#include "util/log.h"

namespace gbc
{
    class GBC
    {
    public:
        GBC(const std::string& file);
        inline CPUData get_cpu_data() const { return m_cpu.get_cpu_data(); }
        inline void step() { m_cpu.step(); }
        inline u16 get_pc() const { return m_cpu.get_pc(); }
        inline u16 get_sp() const { return m_cpu.get_sp(); }
        inline u8 peek_byte(u16 addr) const { return m_bus.peek_byte(addr); }
        inline const u8* get_vram() const { return m_bus.get_vram(); }
        inline const u8* get_wram() const { return m_bus.get_wram(); }
        inline const u8* get_hram() const { return m_bus.get_hram(); }
        inline const u8* get_rom() const { return m_cartridge->get_rom(); }

    private:
        Scope<Cartridge> m_cartridge;
        Bus m_bus;
        CPU m_cpu;
    };
}
