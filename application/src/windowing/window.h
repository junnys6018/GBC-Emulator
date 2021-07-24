#pragma once
#include "common.h"
#include "windowing/input.h"
#include <GLFW/glfw3.h>

namespace app
{
    class Window
    {
    public:
        Window(const char* title, u32 w, u32 h, bool resizable);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        ~Window();

        std::pair<u32, u32> get_size();
        void swap_buffers();

    public:
        Input m_input;
        GLFWwindow* m_handle;
    };
}