/*
    How these tests work:

    01-special.gb
    Blarggs testing framework has 2 notable routines, test_passed and test_failed
    we step the emulation one instruction at a time and check if the program counter
    has reached the address of either routine. We then report the result of the test based
    off whether the program counter hit the passed or failed routine.

    If a test failed, we need to report additional infomation, the result code, and the name
    of the test. Luckily this infomation is embedded in test_failed routine,
    which looks somethine like this:

    test_failed:
         ld   a,(test_name)
         ld   l,a
         ld   a,(test_name+1)
         ld   h,a
         ld   a,(hl)
         or   a
         jr   z,+
         call print_newline
         call print_str_hl
         call print_newline
    +
         ld   a,(result)
         cp   1         ; if a = 0 then a = 1
         adc  0
         jp   exit

    test_name is a pointer to a const char* and result is a uint8_t*,
    they are the immedate data of the ld instruction, so we simply read a fixed offset
    from the test_failed routine to get these values.

    All other tests work differently from 01-special.gb, we still check if a test has passed
    in the same way, however test_failed is only called once all the tests are completed,
    at which point it is very difficult to report which particular test has failed. These roms
    work by testing each instruction one at a time, for each instruction different inputs are tested
    and the registers after executing each instruction are checksummed. The final checksum is then
    compared with the expected value, the program then branches based off the result of the comparison
    we simply need to check if the program ever takes the failed branch, at which point we
    report of the opcode of the instruction that failed. The opcode of the currently executing instruction
    is always storted at 0xDEF8

*/

#include <catch_amalgamated.hpp>
#include <gbc.h>
using namespace gbc;

const u16 RESULT_ADDR_OFFSET = 21;
const u16 INSTR_ADDR = 0xDEF8;
const u16 PASSED_ROUTINE_ADDR = 0xC18B;
const u16 FAILED_INSTR_CHECK = 0xC4E8;
const u16 FAILED_ROUTINE_ADDR = 0xC1B9;

// clang-format off
const char* roms[] = {
    "roms/blargg/03-op sp,hl.gb",
    "roms/blargg/04-op r,imm.gb",
    "roms/blargg/05-op rp.gb",
    "roms/blargg/06-ld r,r.gb",
    "roms/blargg/07-jr,jp,call,ret,rst.gb",
    "roms/blargg/08-misc instrs.gb",
    "roms/blargg/09-op r,r.gb",
    "roms/blargg/10-bit ops.gb",
    "roms/blargg/11-op a,(hl).gb"
};
// clang-format on

#define NUM_TESTS (sizeof(roms) / sizeof(char*))

u16 get_operand_16(u16 addr, const GBC& gbc)
{
    u16 operand = gbc.peek_byte(addr + 1);
    operand |= (static_cast<u16>(gbc.peek_byte(addr + 2)) << 8);
    return operand;
}

u16 get_u16(u16 addr, const GBC& gbc)
{
    u16 operand = gbc.peek_byte(addr);
    operand |= (static_cast<u16>(gbc.peek_byte(addr + 1)) << 8);
    return operand;
}

std::string get_filename(const std::string& path)
{
    size_t backslash = path.find_last_of('/');
    if (backslash == std::string::npos)
        return path;
    return path.substr(backslash + 1);
}

bool test_rom(const char* test)
{
    auto filename = get_filename(test);
    Scope<GBC> gbc = create_scope<GBC>(test);
    u16 passed_routine = PASSED_ROUTINE_ADDR;
    u16 failed_routine = FAILED_INSTR_CHECK;
    u32 steps = 0;

    while (gbc->get_pc() != failed_routine && gbc->get_pc() != passed_routine)
    {
        gbc->step();
        steps++;
    }

    if (gbc->get_pc() == failed_routine)
    {
        u8 opcode1 = gbc->peek_byte(INSTR_ADDR);
        u8 opcode2 = gbc->peek_byte(INSTR_ADDR + 1);
        u8 opcode3 = gbc->peek_byte(INSTR_ADDR + 2);

        LOG_ERROR("Testing {}... Failed! steps: {} opcode: {:2X} {:2X} {:2X}\n", filename, steps, opcode1, opcode2, opcode3);
        return false;
    }
    else
    {
        LOG_INFO("Testing {}... Passed!", filename);
        return true;
    }
}

bool test_special()
{
    const char* filename = "01-special.gb";
    Scope<GBC> gbc = create_scope<GBC>("roms/blargg/01-special.gb");
    u16 passed_routine = PASSED_ROUTINE_ADDR;
    u16 failed_routine = FAILED_ROUTINE_ADDR;
    u32 steps = 0;
    while (gbc->get_pc() != failed_routine && gbc->get_pc() != passed_routine)
    {
        gbc->step();
        steps++;
    }
    if (gbc->get_pc() == failed_routine)
    {
        u16 test_name_addr = get_operand_16(failed_routine, *gbc);
        test_name_addr = get_u16(test_name_addr, *gbc);
        u16 result_addr = get_operand_16(failed_routine + RESULT_ADDR_OFFSET, *gbc);
        u8 result = gbc->peek_byte(result_addr);

        char test_name[64];
        memset(test_name, 0, sizeof(test_name));
        int i = 0;
        while (u8 ch = gbc->peek_byte(test_name_addr++))
        {
            test_name[i++] = gbc::bit_cast<char>(ch);
        }
        LOG_ERROR("Testing {}... Failed! test_name: {} result: {} steps: {}\n", filename, test_name, result, steps);
        return false;
    }
    else
    {
        LOG_INFO("Testing {}... Passed!", filename);
        return true;
    }
}

TEST_CASE("Blargg cpu_instr tests", "[cpu][blargg]")
{
    CHECK(test_special());
    for (int i = 0; i < NUM_TESTS; i++)
    {
        CHECK(test_rom(roms[i]));
    }
}