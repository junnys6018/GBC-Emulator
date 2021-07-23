#include <glad/glad.h>

#include "util/log.h"
#include "windowing/window.h"
#include "windowing/window_manager.h"

namespace app
{
    Window::Window(const char* title, u32 w, u32 h, bool resizable)
    {
        glfwWindowHint(GLFW_RESIZABLE, resizable);
        m_handle = glfwCreateWindow(w, h, title, NULL, NULL);
        if (m_handle == NULL)
        {
            LOG_ERROR("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_handle); // this is bad
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG_ERROR("Failed to initialize OpenGL context");
            exit(EXIT_FAILURE);
        }

        // Define the viewport dimensions
        glViewport(0, 0, w, h);

        WindowManager::add(this);
        m_input.set_window(m_handle);
    }
    Window::~Window() { WindowManager::remove(this); }

    void Window::swap_buffers() { glfwSwapBuffers(m_handle); }
}