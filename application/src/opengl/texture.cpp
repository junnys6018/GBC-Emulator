#include "opengl/texture.h"

namespace app
{
    Texture::Texture(u32 w, u32 h, void* pixels, GLenum format) : m_width(w), m_height(h), m_format(format)
    {
        glGenTextures(1, &m_handle);
        glBindTexture(GL_TEXTURE_2D, m_handle);

        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    Texture::~Texture()
    {
        if (m_handle)
            glDeleteTextures(1, &m_handle);
    }
    Texture::Texture(Texture&& other) : m_width(other.m_width), m_height(other.m_height), m_handle(other.m_handle) { other.m_handle = 0; }
    Texture& Texture::operator=(Texture&& other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_handle = other.m_handle;
        other.m_handle = 0;
        return *this;
    }
    void Texture::set_data(void* pixels, u32 xoff, u32 yoff, u32 w, u32 h)
    {
        glBindTexture(GL_TEXTURE_2D, m_handle);
        if (w == 0)
            w = m_width;
        if (h == 0)
            h = m_height;
        glTexSubImage2D(GL_TEXTURE_2D, 0, xoff, yoff, w, h, m_format, GL_UNSIGNED_BYTE, pixels);
    }
}