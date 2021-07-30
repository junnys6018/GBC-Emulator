#pragma once
#include "common.h"
#include <gbc.h>

namespace app
{
    using namespace gbc;
    class IORegistersWindow
    {
    public:
        IORegistersWindow() = default;
        void draw_window(const char* title, const GBC& gbc);
    };
}