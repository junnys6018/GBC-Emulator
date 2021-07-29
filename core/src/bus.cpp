#include "bus.h"
#include "util/log.h"
#define LOG_WARN(...) (void)0
namespace gbc
{
    Bus::Bus(Cartridge* cartridge) : m_cartridge(cartridge)
    {
        m_vram.fill(0);
        m_wram.fill(0);
        m_hram.fill(0);
        m_oam.fill(0);
    }

    u8 Bus::cpu_read_byte(u16 addr)
    {
        if (addr >= 0x0000 && addr < 0x8000)
        {
            return m_cartridge->read_cartridge(addr);
        }
        else if (addr >= 0x8000 && addr < 0xA000)
        {
            addr &= 0x1FFF;
            return m_vram[addr];
        }
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            return m_cartridge->read_cartridge(addr);
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
            LOG_TRACE("Attempt to read prohibited address {0:x}", addr);
            return 0;
        }
        else if (addr >= 0xFF00 && addr < 0xFF80)
        {
            // TODO: IO registers
            LOG_WARN("read to unimplemented address (IO Registers): {0:x}", addr);
            return 0;
        }
        else if (addr >= 0xFF80 && addr < 0xFFFF) // high ram
        {
            addr &= 0x007F;
            return m_hram[addr];
        }
        else // addr = 0xFFFF
        {
            LOG_WARN("read to unimplemented address (IE Register): {0:x}", addr);
            return 0;
        }
    }

    void Bus::cpu_write_byte(u16 addr, u8 byte)
    {
        if (addr >= 0x0000 && addr < 0x8000)
        {
            m_cartridge->write_cartridge(addr, byte);
        }
        else if (addr >= 0x8000 && addr < 0xA000)
        {
            addr &= 0x1FFF;
            m_vram[addr] = byte;
        }
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            m_cartridge->write_cartridge(addr, byte);
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
            LOG_TRACE("Attempt to write to prohibited address {0:x}", addr);
        }
        else if (addr >= 0xFF00 && addr < 0xFF80)
        {
            // TODO: IO registers
            LOG_WARN("write to unimplemented address (IO Registers): {0:x}", addr);
        }
        else if (addr >= 0xFF80 && addr < 0xFFFF) // high ram
        {
            addr &= 0x007F;
            m_hram[addr] = byte;
        }
        else // addr = 0xFFFF
        {
            LOG_WARN("write to unimplemented address (IE Register): {0:x}", addr);
        }
    }
    u8 Bus::peek_byte(u16 addr) const
    {
        if (addr >= 0x0000 && addr < 0x8000)
        {
            return m_cartridge->peek_cartridge(addr);
        }
        else if (addr >= 0x8000 && addr < 0xA000)
        {
            addr &= 0x1FFF;
            return m_vram[addr];
        }
        else if (addr >= 0xA000 && addr < 0xC000)
        {
            return m_cartridge->peek_cartridge(addr);
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
            return peek_byte(addr);
        }
        else if (addr >= 0xFE00 && addr < 0xFEA0) // oam
        {
            addr &= 0x9F;
            return m_oam[addr];
        }
        else if (addr >= 0xFEA0 && addr < 0xFF00)
        {
            return 0;
        }
        else if (addr >= 0xFF00 && addr < 0xFF80)
        {
            return 0;
        }
        else if (addr >= 0xFF80 && addr < 0xFFFF) // high ram
        {
            addr &= 0x007F;
            return m_hram[addr];
        }
        else // addr = 0xFFFF
        {
            return 0;
        }
    }
}