#pragma once
#include "util/common.h"

namespace gbc
{

    struct HeaderInfo
    {
        char title[17];
        char manufacturer_code[5];
        u8 gbc_flag;
        char licensee_code[2];
        u8 sgb_flag;
        u8 mbc_type;
        u8 rom_size;
        u8 ram_size;
        u8 destination_code;
        u8 old_licensee_code;
        u8 version_number;
        u8 header_checksum;
        u16 global_checksum;
    };

    void log_header_info(const HeaderInfo& hi);
    bool verify_header(const HeaderInfo& hi, std::string& message);
    HeaderInfo parse_header(const std::vector<u8>& rom);
    u32 get_ram_size(const HeaderInfo& hi);

    class Cartridge
    {
    public:
        virtual void write_cartridge(u16 addr, u8 data) = 0;
        virtual u8 read_cartridge(u16 addr) = 0;
        virtual u8 peek_cartridge(u16 addr) const = 0;
        static Scope<Cartridge> from_rom(const std::vector<u8>& rom);
        static Scope<Cartridge> from_rom(std::vector<u8>&& rom);
        inline const u8* get_rom() const { return m_rom.data(); }

    protected:
        Cartridge(const std::vector<u8>& rom, const HeaderInfo& header_info);
        Cartridge(std::vector<u8>&& rom, const HeaderInfo& header_info);

    protected:
        HeaderInfo m_header_info;
        const std::vector<u8> m_rom;
        std::vector<u8> m_ram;

    private:
        void initialize();
    };
}