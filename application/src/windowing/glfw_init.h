#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace app
{
    class Glfw
    {
    public:
        static GLFWwindow* initialize();
        static void shutdown();
        static void begin();
        static void end();

    public:
        static GLFWwindow* s_window;
    };
}