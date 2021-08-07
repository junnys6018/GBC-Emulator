#include "cartridge.h"
#include "util/log.h"

#include "mbc/mbc1.h"
#include "mbc/rom.h"

namespace gbc
{
    Cartridge::Cartridge(const std::vector<u8>& rom, const HeaderInfo& header_info)
        : m_header_info(header_info), m_rom(rom), m_ram(get_ram_size(header_info))
    {
        initialize();
    }
    Cartridge::Cartridge(std::vector<u8>&& rom, const HeaderInfo& header_info)
        : m_header_info(header_info), m_rom(std::move(rom)), m_ram(get_ram_size(header_info))
    {
        initialize();
    }

    void Cartridge::initialize() {}

    static u8 get_hex(char code)
    {
        if (code >= '0' && code <= '9')
            return code - '0';
        else if (code == 'A')
            return 0xA;
        else
        {
            CORE_LOG_WARN("Unknown code: {} passed to get_hex()", code);
            return 0;
        }
    }

    static const char* get_licensee_code(const char code[2])
    {
        u8 hex_code = (get_hex(code[0]) << 4) | get_hex(code[1]);
        switch (hex_code)
        {
        case 0x00: return "None";
        case 0x01: return "Nintendo R&D1";
        case 0x08: return "Capcom";
        case 0x13: return "Electronic Arts";
        case 0x18: return "Hudson Soft";
        case 0x19: return "b-ai";
        case 0x20: return "kss";
        case 0x22: return "pow";
        case 0x24: return "PCM Complete";
        case 0x25: return "san-x";
        case 0x28: return "Kemco Japan";
        case 0x29: return "seta";
        case 0x30: return "Viacom";
        case 0x31: return "Nintendo";
        case 0x32: return "Bandai";
        case 0x33: return "Ocean/Acclaim";
        case 0x34: return "Konami";
        case 0x35: return "Hector";
        case 0x37: return "Taito";
        case 0x38: return "Hudson";
        case 0x39: return "Banpresto";
        case 0x41: return "Ubi Soft";
        case 0x42: return "Atlus";
        case 0x44: return "Malibu";
        case 0x46: return "angel";
        case 0x47: return "Bullet-Proof";
        case 0x49: return "irem";
        case 0x50: return "Absolute";
        case 0x51: return "Acclaim";
        case 0x52: return "Activision";
        case 0x53: return "American sammy";
        case 0x54: return "Konami";
        case 0x55: return "Hi tech entertainment";
        case 0x56: return "LJN";
        case 0x57: return "Matchbox";
        case 0x58: return "Mattel";
        case 0x59: return "Milton Bradley";
        case 0x60: return "Titus";
        case 0x61: return "Virgin";
        case 0x64: return "LucasArts";
        case 0x67: return "Ocean";
        case 0x69: return "Electronic Arts";
        case 0x70: return "Infogrames";
        case 0x71: return "Interplay";
        case 0x72: return "Broderbund";
        case 0x73: return "sculptured";
        case 0x75: return "sci";
        case 0x78: return "THQ";
        case 0x79: return "Accolade";
        case 0x80: return "misawa";
        case 0x83: return "lozc";
        case 0x86: return "Tokuma Shoten Intermedia";
        case 0x87: return "Tsukuda Original";
        case 0x91: return "Chunsoft";
        case 0x92: return "Video system";
        case 0x93: return "Ocean/Acclaim";
        case 0x95: return "Varie";
        case 0x96: return "Yonezawa/s’pal";
        case 0x97: return "Kaneko";
        case 0x99: return "Pack in soft";
        case 0xA4: return "Konami (Yu-Gi-Oh!)";
        default: return "Unknown";
        };
    }

    static const char* get_cartridge_type(u8 code)
    {
        switch (code)
        {
        case 0x00: return "ROM ONLY";
        case 0x01: return "MBC1";
        case 0x02: return "MBC1+RAM";
        case 0x03: return "MBC1+RAM+BATTERY";
        case 0x05: return "MBC2";
        case 0x06: return "MBC2+BATTERY";
        case 0x08: return "ROM+RAM";
        case 0x09: return "ROM+RAM+BATTERY";
        case 0x0B: return "MMM01";
        case 0x0C: return "MMM01+RAM";
        case 0x0D: return "MMM01+RAM+BATTERY";
        case 0x0F: return "MBC3+TIMER+BATTERY";
        case 0x10: return "MBC3+TIMER+RAM+BATTERY";
        case 0x11: return "MBC3";
        case 0x12: return "MBC3+RAM";
        case 0x13: return "MBC3+RAM+BATTERY";
        case 0x19: return "MBC5";
        case 0x1A: return "MBC5+RAM";
        case 0x1B: return "MBC5+RAM+BATTERY";
        case 0x1C: return "MBC5+RUMBLE";
        case 0x1D: return "MBC5+RUMBLE+RAM";
        case 0x1E: return "MBC5+RUMBLE+RAM+BATTERY";
        case 0x20: return "MBC6";
        case 0x22: return "MBC7+SENSOR+RUMBLE+RAM+BATTERY";
        case 0xFC: return "POCKET CAMERA";
        case 0xFD: return "BANDAI TAMA5";
        case 0xFE: return "HuC3";
        case 0xFF: return "HuC1+RAM+BATTERY";
        default: return "Unknown";
        }
    }

#define FROM_ROM_IMPL(fn)                                                                                                                  \
    HeaderInfo hi = parse_header(rom);                                                                                                     \
                                                                                                                                           \
    std::string message;                                                                                                                   \
    if (!verify_header(hi, message))                                                                                                       \
    {                                                                                                                                      \
        CORE_LOG_ERROR("Failed to load rom: {}", message);                                                                                      \
        return nullptr;                                                                                                                    \
    }                                                                                                                                      \
    if (!s_silent)                                                                                                                         \
        log_header_info(hi);                                                                                                               \
    switch (hi.mbc_type)                                                                                                                   \
    {                                                                                                                                      \
    case 0x00: /* ROM only*/ return create_scope<ROM>(fn(rom), hi);                                                                        \
    case 0x01:                                                                                                                             \
    case 0x02:                                                                                                                             \
    case 0x03: /* MBC1 */ return create_scope<MBC1>(fn(rom), hi);                                                                          \
    default: /* Capture unimplemented mbc's */                                                                                             \
        CORE_LOG_ERROR("Not yet implemented mbc: {}", get_cartridge_type(hi.mbc_type));                                                    \
        return nullptr;                                                                                                                    \
    }

    Scope<Cartridge> Cartridge::from_rom(const std::vector<u8>& rom) { FROM_ROM_IMPL(); }
    Scope<Cartridge> Cartridge::from_rom(std::vector<u8>&& rom) { FROM_ROM_IMPL(std::move); }

    void log_header_info(const HeaderInfo& hi)
    {
        CORE_LOG_INFO("title: {}", hi.title);
        CORE_LOG_INFO("gbc flag: {}", (hi.gbc_flag & 0x80) ? "Enabled" : "Disabled");
        CORE_LOG_INFO("sgb flag: {}", (hi.sgb_flag == 0x03) ? "Enabled" : "Disabled");
        if (hi.old_licensee_code == 0x33) // New Licensee code
        {
            CORE_LOG_INFO("licensee: {}", get_licensee_code(hi.licensee_code));
        }
        else // Old Licensee code
        {
            CORE_LOG_INFO("licensee: {0:x}", hi.old_licensee_code);
        }
        CORE_LOG_INFO("cartridge type: {}", get_cartridge_type(hi.mbc_type));
        CORE_LOG_INFO("rom size: {}KB ({} banks)", 32 * (1 << hi.rom_size), 2 * (1 << hi.rom_size));
        u32 ram_banks = get_ram_size(hi) >> 13;
        CORE_LOG_INFO("ram size: {}KB ({} banks)", 8 * ram_banks, ram_banks);
        CORE_LOG_INFO("destination code: {}", (hi.destination_code == 0) ? "Japan" : "International");
        CORE_LOG_INFO("version number: {}", hi.version_number);
        CORE_LOG_INFO("header checksum: 0x{0:02x}", hi.header_checksum);
        CORE_LOG_INFO("global checksum: 0x{0:04x}", hi.global_checksum);
    }

    bool verify_header(const HeaderInfo& hi, std::string& message)
    {
        // TODO
        static std::set<u8> allowed_cartridge_types = {0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x08, 0x09, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11,
                                                       0x12, 0x13, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x20, 0x22, 0xFC, 0xFD, 0xFE, 0xFF};
        static std::set<u8> allowed_ram_sizes = {0, 2, 3, 4, 5};

        if (allowed_cartridge_types.find(hi.mbc_type) == allowed_cartridge_types.end())
        {
            message = "Invalid value at 0x147 - cartridge type";
            return false;
        }

        if (allowed_ram_sizes.find(hi.ram_size) == allowed_ram_sizes.end())
        {
            message = "Invalid value at 0x149 - ram size";
            return false;
        }
        return true;
    }

    HeaderInfo parse_header(const std::vector<u8>& rom)
    {
        HeaderInfo hi;
        std::memcpy(hi.title, rom.data() + 0x134, 16);
        hi.title[16] = '\0';

        std::memcpy(hi.manufacturer_code, rom.data() + 0x13F, 4);
        hi.manufacturer_code[4] = '\0';

        hi.gbc_flag = rom[0x143];
        std::memcpy(hi.licensee_code, rom.data() + 0x144, 2);
        hi.sgb_flag = rom[0x146];
        hi.mbc_type = rom[0x147];
        hi.rom_size = rom[0x148];
        hi.ram_size = rom[0x149];
        hi.destination_code = rom[0x14A];
        hi.old_licensee_code = rom[0x14B];
        hi.version_number = rom[0x14C];
        hi.header_checksum = rom[0x14D];
        hi.global_checksum = (static_cast<u16>(rom[0x14E]) << 8);
        hi.global_checksum |= static_cast<u16>(rom[0x14F]);

        return hi;
    }

    u32 get_ram_size(const HeaderInfo& hi)
    {
        switch (hi.ram_size)
        {
        case 0: return 0 * 8192;
        case 2: return 1 * 8192;
        case 3: return 4 * 8192;
        case 4: return 16 * 8192;
        case 5: return 8 * 8192;
        }
        ASSERT(false); // Unreachable code
        return 0;
    }
}