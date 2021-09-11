#include "lcd_renderer.h"

namespace app
{
    static GLuint compile_shader(const char* src, GLenum shader_type)
    {
        GLuint shader = glCreateShader(shader_type);

        glShaderSource(shader, 1, &src, NULL);

        glCompileShader(shader);

        // Error handling
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            if (length != 0)
            {
                std::vector<GLchar> infolog(length);
                glGetShaderInfoLog(shader, length, NULL, &infolog[0]);
                CLIENT_LOG_ERROR("failed to compile shader: {}", infolog.data());
            }
        }

        return shader;
    }

    static GLuint build_shader(const char* vertex_src, const char* fragment_src)
    {
        GLuint vertex_shader = compile_shader(vertex_src, GL_VERTEX_SHADER);
        GLuint fragment_shader = compile_shader(fragment_src, GL_FRAGMENT_SHADER);

        GLuint program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        glLinkProgram(program);

        // Error handling
        glValidateProgram(program);
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            if (length != 0)
            {
                std::vector<GLchar> infolog(length);
                glGetShaderInfoLog(program, length, NULL, &infolog[0]);
                CLIENT_LOG_ERROR("failed to link shader: {}", infolog.data());
            }
        }

        // Cleanup
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return program;
    }

    LCDRenderer::LCDRenderer() : m_texture(160, 144, nullptr, GL_RGBA)
    {
        // create vertex buffer and vertex array object
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        // clang-format off
        float vertices[] = {
            -1.0, -1.0, 0.0, 1.0,
             1.0, -1.0, 1.0, 1.0,
             1.0,  1.0, 1.0, 0.0,

             1.0,  1.0, 1.0, 0.0,
            -1.0,  1.0, 0.0, 0.0,
            -1.0, -1.0, 0.0, 1.0,
        };
        // clang-format on

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // create shader

        static const char* vertex_src = "#version 330 core\n"
                                        "layout(location = 0) in vec2 a_position;"
                                        "layout(location = 1) in vec2 a_texcoord;"
                                        "out vec2 v_texcoord;"
                                        "void main() {"
                                        "    gl_Position = vec4(a_position, 0.0, 1.0);"
                                        "    v_texcoord = a_texcoord;"
                                        "}";

        static const char* fragment_src = "#version 330 core\n"
                                          "in vec2 v_texcoord;"
                                          "uniform sampler2D u_texture;"
                                          "out vec4 color;"
                                          "void main() {"
                                          "    color = texture(u_texture, v_texcoord);\n"
                                          "}";

        m_shader = build_shader(vertex_src, fragment_src);

        // set texture uniform
        glUseProgram(m_shader);
        glUniform1i(glGetUniformLocation(m_shader, "u_texture"), 0);
    }

    LCDRenderer::~LCDRenderer()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteProgram(m_shader);
    }

    void LCDRenderer::render(const u32* pixel_data)
    {
        glUseProgram(m_shader);
        glBindVertexArray(m_vao);
        m_texture.set_data(pixel_data);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}