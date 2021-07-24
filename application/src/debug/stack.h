#pragma once
#include <gbc.h>

namespace app
{
    using namespace gbc;
    class StackWindow
    {
    public:
        StackWindow() = default;
        void draw_window(const char* title, const GBC& gbc);
        i32 m_lines = 8;
    };
}