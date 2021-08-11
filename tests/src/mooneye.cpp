#include "common.h"
#include <catch_amalgamated.hpp>
#include <gbc.h>
using namespace gbc;

// clang-format off
static const char* mbc_roms[] = {
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

static const char* timer_roms[] = {
    "roms/mooneye-gb_hwtests/acceptance/timer/div_write.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/rapid_toggle.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/tim00.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/tim00_div_trigger.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/tim01.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/tim01_div_trigger.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/tim10.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/tim10_div_trigger.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/tim11.gb",
    "roms/mooneye-gb_hwtests/acceptance/timer/tim11_div_trigger.gb",
    //"roms/mooneye-gb_hwtests/acceptance/timer/tima_reload.gb",
    //"roms/mooneye-gb_hwtests/acceptance/timer/tima_write_reloading.gb",
    //"roms/mooneye-gb_hwtests/acceptance/timer/tma_write_reloading.gb",
};

static const char* timer_tilemaps[] = {
    "tilemaps/test_ok.bin",
    "tilemaps/rapid_toggle.bin",
    "tilemaps/tim00.bin",
    "tilemaps/tim00_div_trigger.bin",
    "tilemaps/tim01.bin",
    "tilemaps/tim01_div_trigger.bin",
    "tilemaps/tim10.bin",
    "tilemaps/tim10_div_trigger.bin",
    "tilemaps/tim11.bin",
    "tilemaps/tim11_div_trigger.bin",
    //"tilemaps/tima_reload.gb",
    //"tilemaps/tima_write_reloading.gb",
    //"tilemaps/tma_write_reloading.gb",
};

static_assert(LENGTH(timer_roms) == LENGTH(timer_tilemaps));
// clang-format on

bool test_mooneye(const std::string& rom, const std::vector<u8>& expected_tilemap, float time_seconds = 30)
{
    Scope<GBC> gbc = create_scope<GBC>(rom);
    const int cycles = time_seconds * MASTER_CLOCK_FREQ;
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
    auto expected_framebuffer = read_file("tilemaps/test_ok.bin");
    for (int i = 0; i < LENGTH(mbc_roms); i++)
    {
        CHECK(test_mooneye(mbc_roms[i], expected_framebuffer));
    }
}

TEST_CASE("Mooneye Timer", "[mooneye][Timer]")
{
    for (int i = 0; i < LENGTH(timer_roms); i++)
    {
        auto expected_framebuffer = read_file(timer_tilemaps[i]);
        CHECK(test_mooneye(timer_roms[i], expected_framebuffer, 10));
    }
}