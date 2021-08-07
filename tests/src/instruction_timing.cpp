#include "gbc_access.h"
#include <catch_amalgamated.hpp>
#include <gbc.h>
using namespace gbc;
// clang-format off
u32 opcode_timings[] = {
    1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,
    0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
    2,3,2,2,1,1,2,1,2,2,2,2,1,1,2,1,
    2,3,2,2,3,3,3,1,2,2,2,2,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
    2,3,3,4,3,4,2,4,2,4,3,0,3,6,2,4,
    2,3,3,0,3,4,2,4,2,4,3,0,3,0,2,4,
    3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,
    3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4
};

u32 cb_opcode_timings[] = {
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
    2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
    2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
    2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
    2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2
};
// clang-format on

bool GBCTests::test_instruction_timings()
{
    // These opcodes have variable execution times, we dont bother testing those
    static std::set<u32> branch_opcodes = {0x20, 0x28, 0x30, 0x38, 0xC0, 0xC2, 0xC4, 0xC8, 0xCA, 0xCC, 0xD0, 0xD2, 0xD4, 0xD8, 0xDA, 0xDC};

    Scope<GBC> gbc = create_scope<GBC>("roms/blargg/01-special.gb"); // Load any rom, we dont care
    for (u32 opcode = 0; opcode < 256; opcode++)
    {
        if (opcode_timings[opcode] == 0) // illegal/halt/stop opcodes
            continue;
        if (branch_opcodes.find(opcode) != branch_opcodes.end())
            continue;

        Operation operation = CPU::s_opcodes[opcode];
        u32 cycles = (gbc->m_cpu.*operation)();
        if (cycles != opcode_timings[opcode])
        {
            CLIENT_LOG_ERROR("failed opcode {:2X} cycles={}", opcode, cycles);
            return false;
        }
    }
    // cb prefixed opcodes
    for (u32 opcode = 0; opcode < 256; opcode++)
    {
        Operation operation = CPU::s_cb_opcodes[opcode];
        u32 cycles = (gbc->m_cpu.*operation)();
        if (cycles != cb_opcode_timings[opcode])
        {
            CLIENT_LOG_ERROR("failed opcode CB {:2X} cycles={}", opcode, cycles);
            return false;
        }
    }

    return true;
}

bool test_blargg_instr_timing()
{
    const char* filename = "instr_timing.gb";
    Scope<GBC> gbc = create_scope<GBC>("roms/blargg/instr_timing.gb");
    const u16 failed_routine = 0xC1B9, passed_routine = 0xC18B;
    u32 steps = 0;

    while (gbc->get_pc() != failed_routine && gbc->get_pc() != passed_routine)
    {
        gbc->step();
        steps++;
    }
    if (gbc->get_pc() == failed_routine)
    {
        CLIENT_LOG_ERROR("Testing {}... Failed! steps={}", filename, steps);
        return false;
    }
    else
    {
        CLIENT_LOG_INFO("Testing {}... Passed!", filename);
        return true;
    }
}

TEST_CASE("instruction_timings", "[cpu]")
{
    CHECK(GBCTests::test_instruction_timings());
    CHECK(test_blargg_instr_timing());
}