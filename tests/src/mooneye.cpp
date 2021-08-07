#include "common.h"
#include <catch_amalgamated.hpp>
#include <gbc.h>
using namespace gbc;

// clang-format off
static const char* roms[] = {
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/bits_bank1.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/bits_bank2.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/bits_mode.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/bits_ramg.gb",
    //"roms/mooneye-gb_hwtests/emulator-only/mbc1/multicart_rom_8Mb.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/ram_64kb.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/ram_256kb.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/rom_1Mb.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/rom_2Mb.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/rom_4Mb.gb",
    //"roms/mooneye-gb_hwtests/emulator-only/mbc1/rom_8Mb.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/rom_16Mb.gb",
    "roms/mooneye-gb_hwtests/emulator-only/mbc1/rom_512kb.gb",
};
// clang-format on

#define NUM_TESTS (sizeof(roms) / sizeof(char*))

bool test_mooneye(const std::string& rom, const std::vector<u8>& expected_tilemap)
{
    Scope<GBC> gbc = create_scope<GBC>(rom);
    // Emulate for 30secs
    const int cycles = 30 * MASTER_CLOCK_FREQ;
    for (int i = 0; i < cycles; i++)
    {
        gbc->clock();
    }
    auto tilemap = gbc->dump_bg_tile_map();
    bool result = tilemap == expected_tilemap;
    auto filename = get_filename(rom);
    if (result)
        CLIENT_LOG_INFO("Testing {}... Passed!", filename);
    else
        CLIENT_LOG_ERROR("Testing {}... Failed!", filename);

    return result;
}

TEST_CASE("Mooneye MBC1", "[cartridge][mooneye][MBC1]")
{
    auto expected_framebuffer = read_file("res/expected_mooneye_tilemap.bin");
    for (int i = 0; i < NUM_TESTS; i++)
    {
        CHECK(test_mooneye(roms[i], expected_framebuffer));
    }
}