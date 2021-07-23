#pragma once
#include "cartridge.h"
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
    };
}