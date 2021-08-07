#include "opengl/debug.h"
#include <glad/glad.h>
#include <util/log.h>

namespace app
{

    static void opengl_log_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                                   const void* userParam)
    {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH: CLIENT_LOG_ERROR("[OpenGL Debug] {}", message); return;
        case GL_DEBUG_SEVERITY_MEDIUM: CLIENT_LOG_WARN("[OpenGL Debug] {}", message); return;
        case GL_DEBUG_SEVERITY_LOW: CLIENT_LOG_INFO("[OpenGL Debug] {}", message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: return;
        }
    }

    void enable_gl_debugging()
    {
        glDebugMessageCallback(opengl_log_message, NULL);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
}