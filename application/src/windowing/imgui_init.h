#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace app
{
    class ImGuiLayer
    {
    public:
        static void Initialize(GLFWwindow* window);
        static void Shutdown();
        static void Begin();
        static void End();
    };
}