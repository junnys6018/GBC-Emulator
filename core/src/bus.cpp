#include "bus.h"
#include "util/log.h"

namespace gbc
{
    Bus::Bus(Cartridge* cartridge) : m_cartridge(cartridge) {}

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
            LOG_WARN("read to unimplemented address: {0:x}", addr);
            addr &= 0x0FFF;
            return m_wram[addr | 0x1000];
        }
        else if (addr >= 0xE000 && addr < 0xFE00)
        {
            // Mirror of C000-DDFF
            addr = (addr & 0x1FFF) | 0xC000;
            return cpu_read_byte(addr);
        }
        else if (addr >= 0xFE00 && addr < 0xFEA0)
        {
            // TODO: OAM
            LOG_WARN("read to unimplemented address: {0:x}", addr);
        }
        else if (addr >= 0xFEA0 && addr < 0xFF00)
        {
            LOG_INFO("Attempt to read unusable address {0:x}", addr);
            return 0;
        }
        else if (addr >= 0xFF00 && addr < 0xFF80)
        {
            // TODO: IO registers
            LOG_WARN("read to unimplemented address: {0:x}", addr);
        }
        else if (addr >= 0xFF80 && addr < 0xFFFF)
        {
            // high ram
            LOG_WARN("read to unimplemented address: {0:x}", addr);
        }
        else // addr = 0xFFFF
        {
            LOG_WARN("read to unimplemented address: {0:x}", addr);
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
            LOG_WARN("read to unimplemented address: {0:x}", addr);
            addr &= 0x0FFF;
            m_wram[addr | 0x1000] = byte;
        }
        else if (addr >= 0xE000 && addr < 0xFE00)
        {
            // Mirror of C000-DDFF
            addr = (addr & 0x1FFF) | 0xC000;
            return cpu_write_byte(addr, byte);
        }
        else if (addr >= 0xFE00 && addr < 0xFEA0)
        {
            // TODO: OAM
            LOG_WARN("write to unimplemented address: {0:x}", addr);
        }
        else if (addr >= 0xFEA0 && addr < 0xFF00)
        {
            LOG_INFO("Attempt to write unusable address {0:x}", addr);
        }
        else if (addr >= 0xFF00 && addr < 0xFF80)
        {
            // TODO: IO registers
            LOG_WARN("write to unimplemented address: {0:x}", addr);
        }
        else if (addr >= 0xFF80 && addr < 0xFFFF)
        {
            // high ram
            LOG_WARN("write to unimplemented address: {0:x}", addr);
        }
        else // addr = 0xFFFF
        {
            LOG_WARN("write to unimplemented address: {0:x}", addr);
        }
    }
}