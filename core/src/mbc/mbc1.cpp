#include "mbc/mbc1.h"

namespace gbc
{
    MBC1::MBC1(const std::vector<u8>& rom, const HeaderInfo& header_info) : Cartridge(rom, header_info) {}
    MBC1::MBC1(std::vector<u8>&& rom, const HeaderInfo& header_info) : Cartridge(rom, header_info) { LOG_INFO("move"); }

    void MBC1::write_cartridge(u16 addr, u8 data)
    {
        // TODO: bank switching
        LOG_INFO("write: addr = {0:04x}; val = {0:02x}", addr, data);
    }
    u8 MBC1::read_cartridge(u16 addr)
    {
        // TODO
        return m_rom[addr];
    }
    u8 MBC1::peek_cartridge(u16 addr) const
    {
        // TODO
        return m_rom[addr];
    }
}