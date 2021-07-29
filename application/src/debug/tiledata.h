#pragma once
#include "opengl/texture.h"
#include <gbc.h>

namespace app
{
    using namespace gbc;
    class TiledataWindow
    {
    public:
        TiledataWindow();
        void draw_window(const char* title, const GBC& gbc);
        float m_scale = 2;
    private:
        Texture m_texture;
    };
}