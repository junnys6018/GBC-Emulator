#pragma once
#include "opengl/texture.h"
#include <gbc.h>

namespace app
{
    using namespace gbc;
    class LCDWindow
    {
    public:
        LCDWindow();
        void draw_window(const char* title, const GBC& gbc);

    private:
        Texture m_texture;
    };
}