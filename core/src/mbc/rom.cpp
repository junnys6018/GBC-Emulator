#include "mbc/rom.h"

namespace gbc
{
    ROM::ROM(const std::vector<u8>& rom, const HeaderInfo& header_info) : Cartridge(rom, header_info) {}
    ROM::ROM(std::vector<u8>&& rom, const HeaderInfo& header_info) : Cartridge(rom, header_info) {}

    void ROM::write_cartridge(u16 addr, u8 data)
    {
        if (addr >= 0xA000 && addr < 0xC000)
        {
            addr &= 0x1FFF;
            if (addr < m_ram.size())
                m_ram[addr] = data;
            else
                LOG_TRACE("Attempt to write to ram when none exists");
        }
    }

    u8 ROM::read_cartridge(u16 addr)
    {
        if (addr >= 0x0000 && addr < 0x8000)
            return m_rom[addr];
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            addr &= 0x1FFF;
            if (addr < m_ram.size())
                return m_ram[addr];
            else
            {
                LOG_TRACE("Attempt to read from ram when none exists");
                return 0xFF;
            }
        }
        ASSERT(false); // Unreachable code
        return 0;
    }
    u8 ROM::peek_cartridge(u16 addr) const { return const_cast<ROM*>(this)->read_cartridge(addr); }
}