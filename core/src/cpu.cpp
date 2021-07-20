#include "cpu.h"

namespace gbc
{
    struct Instruction
    {
        char mnemonic[5];
        u32 cycles;
    };

    void CPU::clock()
    {
    }

    void CPU::run_until(u64 clock)
    {
    }

    CPU::CPU(const Ref<Bus>& bus) : m_bus(bus)
    {
    }
}