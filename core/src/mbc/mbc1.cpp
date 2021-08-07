#include "mbc/mbc1.h"

namespace gbc
{
    MBC1::MBC1(const std::vector<u8>& rom, const HeaderInfo& header_info) : Cartridge(rom, header_info)
    {
        if (header_info.rom_size == 5)
            CORE_LOG_ERROR("not yet implemented 1MB rom");
    }
    MBC1::MBC1(std::vector<u8>&& rom, const HeaderInfo& header_info) : Cartridge(std::move(rom), header_info)
    {
        if (header_info.rom_size == 5)
            CORE_LOG_ERROR("not yet implemented 1MB rom");
    }

    void MBC1::write_cartridge(u16 addr, u8 data)
    {
        if (addr >= 0x0000 && addr < 0x2000)
        {
            m_ram_enable = data;
        }
        else if (addr >= 0x2000 && addr < 0x4000)
        {
            m_rom_bank_number.set(data);
            CORE_LOG_TRACE("banked to: {}", get_rom_bank());
        }
        else if (addr >= 0x4000 && addr < 0x6000)
        {
            m_romram_bank_number.set(data);
        }
        else if (addr >= 0x6000 && addr < 0x8000)
        {
            m_banking_mode.set(data);
            CORE_LOG_TRACE("banking mode set to: {}", m_banking_mode.get());
        }
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            if (num_ram_banks() == 0)
                return;
            if ((m_ram_enable & 0xF) == 0xA)
            {
                u8 bank = get_ram_bank();
                m_ram[bank * (u64)0x2000 + (addr & 0x1FFF)] = data;
            }
        }
    }
    u8 MBC1::read_cartridge(u16 addr)
    {
        if (addr >= 0x0000 && addr < 0x4000)
        {
            if (m_banking_mode.get() == 0)
                return m_rom[addr];
            else
                return m_rom[(((u64)m_romram_bank_number.get() << 5) & (num_rom_banks() - 1)) * (u64)0x4000 + addr];
        }
        else if (addr >= 0x4000 && addr < 0x8000)
        {
            u8 bank = get_rom_bank();
            return m_rom[bank * (u64)0x4000 + (addr & 0x3FFF)];
        }
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            if (num_ram_banks() == 0)
                return 0xFF;

            if ((m_ram_enable & 0xF) == 0xA)
            {
                u8 bank = get_ram_bank();
                return m_ram[bank * (u64)0x2000 + (addr & 0x1FFF)];
            }
            else
                return 0xFF;
        }
        ASSERT(false);
    }
    u8 MBC1::peek_cartridge(u16 addr) const { return const_cast<MBC1*>(this)->read_cartridge(addr); }

    u8 MBC1::get_rom_bank() const
    {
        u8 bank = (m_romram_bank_number.get() << 5) | m_rom_bank_number.get();

        if ((bank & 0x1F) == 0)
            bank++;
        bank &= (num_rom_banks() - 1);

        return bank;
    }

    u8 MBC1::get_ram_bank() const
    {
        u8 bank = (m_banking_mode.get() ? m_romram_bank_number.get() : 0);
        bank &= (num_ram_banks() - 1);
        return bank;
    }
}