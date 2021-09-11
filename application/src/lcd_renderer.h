#pragma once
#include "common.h"
#include "opengl/texture.h"
#include <glad/glad.h>
#include <gbc.h>

namespace app
{
    class LCDRenderer
    {
    public:
        LCDRenderer();
        ~LCDRenderer();
        void render(const u32* pixel_data);

    private:
        Texture m_texture;
        GLuint m_shader;
        GLuint m_vbo, m_vao;
    };
};