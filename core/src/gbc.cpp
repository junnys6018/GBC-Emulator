#include "gbc.h"

namespace gbc
{
    const char* hello_world()
    {
        return "Hello World from gbc!";
    }

    GBC::GBC() : m_cpu(&m_bus)
    {
    }
}