#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace app
{
    class Glfw
    {
    public:
        static GLFWwindow* Initialize();
        static void Shutdown();
        static void Begin();
        static void End();

    public:
        static GLFWwindow* s_window;
    };
}