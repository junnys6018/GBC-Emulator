#pragma once
#include "GLFW/glfw3.h"
#include "windowing/input.h"

namespace app
{
    class Window;
    class WindowManager
    {
    public:
        static void add(Window* window);
        static void remove(Window* window);
        static Input& get_input(GLFWwindow* window);

    private:
        static std::vector<Window*> s_windows;
    };
}