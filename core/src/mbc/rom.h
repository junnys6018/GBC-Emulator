#pragma once
#include "cartridge.h"
#include "util/log.h"

namespace gbc
{
    class ROM final : public Cartridge
    {
    public:
        ROM(const std::vector<u8>& rom, const HeaderInfo& header_info);
        ROM(std::vector<u8>&& rom, const HeaderInfo& header_info);

        virtual void write_cartridge(u16 addr, u8 data) override;
        virtual u8 read_cartridge(u16 addr) override;
        virtual u8 peek_cartridge(u16 addr) const override;
    };
}