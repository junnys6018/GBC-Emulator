#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "common.h"

namespace app
{
    class Glfw
    {
    public:
        static GLFWwindow* initialize();
        static void shutdown();
        static void begin();
        static void end();
        static std::pair<u32, u32> get_window_size();

    public:
        static GLFWwindow* s_window;
    };
}