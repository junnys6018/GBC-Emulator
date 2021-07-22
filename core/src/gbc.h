#pragma once
#include "bus.h"
#include "cpu.h"
#include "util/log.h"

namespace gbc
{
    const char* hello_world();

    class GBC
    {
    public:
        GBC();
        CPUData get_cpu_data() { return m_cpu.get_cpu_data(); }
        u8* get_memory() { return m_bus.get_memory(); }
        void step() { m_cpu.step(); }

    private:
        Bus m_bus;
        CPU m_cpu;
    };
}
