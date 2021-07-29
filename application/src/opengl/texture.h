#pragma once
#include <glad/glad.h>
#include "common.h"
namespace app
{
    // Simple RAII wrapper for constructing opengl textures
    class Texture
    {
    public:
        Texture(u32 w, u32 h, void* pixels, GLenum format);
        ~Texture();
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other);
        Texture& operator=(Texture&& other);
        void set_data(void* pixels, u32 xoff = 0, u32 yoff = 0, u32 w = 0, u32 h = 0);

        u32 m_width, m_height;
        GLenum m_format;
        GLuint m_handle;
    };
}