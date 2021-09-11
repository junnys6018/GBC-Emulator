#include <glad/glad.h>

#include "util/log.h"
#include "windowing/window.h"
#include "windowing/window_manager.h"

#include "opengl/debug.h"

namespace app
{
    Window::Window(const char* title, u32 w, u32 h, bool resizable)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, resizable);

        m_handle = glfwCreateWindow(w, h, title, NULL, NULL);
        if (m_handle == NULL)
        {
            CLIENT_LOG_ERROR("Failed to create GLFW window");
            return;
        }

        glfwMakeContextCurrent(m_handle);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            CLIENT_LOG_ERROR("Failed to initialize glad loader");
            exit(EXIT_FAILURE);
        }

        enable_gl_debugging();

        // Define the viewport dimensions
        glViewport(0, 0, w, h);

        WindowManager::add(this);
        m_input.set_window(m_handle);
    }

    Window::Window(Window&& other) noexcept : m_input(other.m_input), m_handle(other.m_handle)
    {
        WindowManager::add(this);

        WindowManager::remove(&other);
        other.m_handle = nullptr;
    }

    Window& Window::operator=(Window&& other) noexcept
    {
        m_input = other.m_input;
        m_handle = other.m_handle;
        WindowManager::add(this);

        WindowManager::remove(&other);
        other.m_handle = nullptr;
        return *this;
    }

    Window::~Window()
    {
        if (m_handle)
        {
            WindowManager::remove(this);
            glfwDestroyWindow(m_handle);
        }
    }

    std::pair<u32, u32> Window::get_size()
    {
        i32 w, h;
        glfwGetWindowSize(m_handle, &w, &h);
        return {w, h};
    }
    void Window::swap_buffers() { glfwSwapBuffers(m_handle); }
}