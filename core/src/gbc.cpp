#include "gbc.h"

namespace gbc
{
    GBC::GBC(const std::string& file) : m_cartridge(Cartridge::from_rom(read_file(file))), m_bus(m_cartridge.get()), m_cpu(&m_bus)
    {
    }
}