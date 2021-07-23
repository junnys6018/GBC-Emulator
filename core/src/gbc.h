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
        inline u8 peek_byte(u16 addr) const { return m_bus.peek_byte(addr); }

    private:
        Scope<Cartridge> m_cartridge;
        Bus m_bus;
        CPU m_cpu;
    };
}
