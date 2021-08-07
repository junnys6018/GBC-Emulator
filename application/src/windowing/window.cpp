#include <glad/glad.h>

#include "util/log.h"
#include "windowing/window.h"
#include "windowing/window_manager.h"

#include "opengl/debug.h"

namespace app
{
    Window::Window(const char* title, u32 w, u32 h, bool resizable)
    {
        glfwWindowHint(GLFW_RESIZABLE, resizable);
        m_handle = glfwCreateWindow(w, h, title, NULL, NULL);
        if (m_handle == NULL)
        {
            CLIENT_LOG_ERROR("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_handle); // this is bad
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // TODO: THIS SHOULD NOT BE HERE
        {
            CLIENT_LOG_ERROR("Failed to initialize OpenGL context");
            exit(EXIT_FAILURE);
        }

        enable_gl_debugging();

        // Define the viewport dimensions
        glViewport(0, 0, w, h);

        WindowManager::add(this);
        m_input.set_window(m_handle);
    }
    Window::~Window() { WindowManager::remove(this); }

    std::pair<u32, u32> Window::get_size()
    {
        i32 w, h;
        glfwGetWindowSize(m_handle, &w, &h);
        return {w, h};
    }
    void Window::swap_buffers() { glfwSwapBuffers(m_handle); }
}