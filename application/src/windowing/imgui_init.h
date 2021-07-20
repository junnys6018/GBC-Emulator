#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace app
{
    class ImGuiLayer
    {
    public:
        static void initialize(GLFWwindow* window);
        static void shutdown();
        static void begin();
        static void end();
    };
}