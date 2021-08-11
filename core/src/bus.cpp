#include "bus.h"
#include "gbc.h"
#include "util/log.h"

namespace gbc
{
    Bus::Bus(GBC* gbc) : m_gbc(gbc), m_registers(gbc)
    {
        m_vram.fill(0);
        m_wram.fill(0);
        m_hram.fill(0);
        m_oam.fill(0);
    }

    u8 Bus::cpu_read_byte(u16 addr)
    {
        Cartridge* cartridge = m_gbc->m_cartridge.get();
        if (addr >= 0x0000 && addr < 0x8000)
        {
            return cartridge->read_cartridge(addr);
        }
        else if (addr >= 0x8000 && addr < 0xA000)
        {
            addr &= 0x1FFF;
            return m_vram[addr];
        }
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            return cartridge->read_cartridge(addr);
        }
        else if (addr >= 0xC000 && addr < 0xD000)
        {
            addr &= 0x0FFF;
            return m_wram[addr];
        }
        else if (addr >= 0xD000 && addr < 0xE000)
        {
            // todo: Bank switch, currently fixed to first bank
            addr &= 0x0FFF;
            return m_wram[addr | 0x1000];
        }
        else if (addr >= 0xE000 && addr < 0xFE00)
        {
            // Mirror of C000-DDFF
            addr = (addr & 0x1FFF) | 0xC000;
            return cpu_read_byte(addr);
        }
        else if (addr >= 0xFE00 && addr < 0xFEA0) // oam
        {
            addr &= 0x9F;
            return m_oam[addr];
        }
        else if (addr >= 0xFEA0 && addr < 0xFF00)
        {
            // Nintendo says use of this area is prohibited
            // LOG_ERROR("Attempt to read prohibited address {:4X}", addr);
            return 0;
        }
        else if (addr >= 0xFF00 && addr < 0xFF80) // io registers
        {
            return m_registers.cpu_read_byte(addr);
        }
        else if (addr >= 0xFF80 && addr < 0xFFFF) // high ram
        {
            addr &= 0x007F;
            return m_hram[addr];
        }
        else // addr = 0xFFFF
        {
            return m_registers.m_interrupt_enable;
        }
    }

    void Bus::cpu_write_byte(u16 addr, u8 byte)
    {
        Cartridge* cartridge = m_gbc->m_cartridge.get();
        if (addr >= 0x0000 && addr < 0x8000)
        {
            cartridge->write_cartridge(addr, byte);
        }
        else if (addr >= 0x8000 && addr < 0xA000)
        {
            addr &= 0x1FFF;
            m_vram[addr] = byte;
        }
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            cartridge->write_cartridge(addr, byte);
        }
        else if (addr >= 0xC000 && addr < 0xD000)
        {
            addr &= 0x0FFF;
            m_wram[addr] = byte;
        }
        else if (addr >= 0xD000 && addr < 0xE000)
        {
            // todo: Bank switch, currently fixed to first bank
            addr &= 0x0FFF;
            m_wram[addr | 0x1000] = byte;
        }
        else if (addr >= 0xE000 && addr < 0xFE00)
        {
            // Mirror of C000-DDFF
            addr = (addr & 0x1FFF) | 0xC000;
            return cpu_write_byte(addr, byte);
        }
        else if (addr >= 0xFE00 && addr < 0xFEA0) // oam
        {
            addr &= 0x9F;
            m_oam[addr] = byte;
        }
        else if (addr >= 0xFEA0 && addr < 0xFF00)
        {
            // Nintendo says use of this area is prohibited
            // LOG_ERROR("Attempt to write to prohibited address {:4X}", addr);
        }
        else if (addr >= 0xFF00 && addr < 0xFF80) // io registers
        {
            m_registers.cpu_write_byte(addr, byte);
        }
        else if (addr >= 0xFF80 && addr < 0xFFFF) // high ram
        {
            addr &= 0x007F;
            m_hram[addr] = byte;
        }
        else // addr = 0xFFFF
        {
            m_registers.m_interrupt_enable = byte;
        }
    }

    u8 Bus::ppu_read_byte(u16 addr)
    {
        if (addr >= 0x8000 && addr < 0xA000)
        {
            addr &= 0x1FFF;
            return m_vram[addr];
        }
        else if (addr >= 0xFE00 && addr < 0xFEA0)
        {
            if (m_gbc->m_oam_dma_transfer == 0)
            {
                addr &= 0xFF;
                return m_oam[addr];
            }
            return 0xFF;
        }
        ASSERT(false);
    }
    u8 Bus::peek_byte(u16 addr) const
    {
        Cartridge* cartridge = m_gbc->m_cartridge.get();
        if (addr >= 0x0000 && addr < 0x8000)
        {
            return cartridge->peek_cartridge(addr);
        }
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            return cartridge->peek_cartridge(addr);
        }
        else if (addr >= 0xFF00 && addr < 0xFF80)
        {
            return m_registers.peek_byte(addr);
        }

        // All other addresses are safe
        return const_cast<Bus*>(this)->cpu_read_byte(addr);
    }
}