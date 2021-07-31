#pragma once
#include "cartridge.h"
#include "util/clipped_register.h"
#include "util/log.h"

namespace gbc
{
    class MBC1 final : public Cartridge
    {
    public:
        MBC1(const std::vector<u8>& rom, const HeaderInfo& header_info);
        MBC1(std::vector<u8>&& rom, const HeaderInfo& header_info);

        virtual void write_cartridge(u16 addr, u8 data) override;
        virtual u8 read_cartridge(u16 addr) override;
        virtual u8 peek_cartridge(u16 addr) const override;

    private:
        u8 m_ram_enable = 0x00;                   // 0000-1FFF
        ClippedU8<5> m_rom_bank_number = 0x01;    // 2000-3FFF
        ClippedU8<2> m_romram_bank_number = 0x00; // 4000-5FFF
        ClippedU8<1> m_banking_mode = 0x00;       // 6000-7FFF

        inline u8 get_rom_bank() const;
        inline u8 get_ram_bank() const;
    };
}