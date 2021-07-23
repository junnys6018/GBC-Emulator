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
        CPUData get_cpu_data() { return m_cpu.get_cpu_data(); }
        void step() { m_cpu.step(); }

    private:
        Scope<Cartridge> m_cartridge;
        Bus m_bus;
        CPU m_cpu;
    };
}
