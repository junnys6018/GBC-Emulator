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

    public:
        // Global event emitters, will emit an event if any window recieved that event
        static EventEmitter<i32, i32, i32> s_on_key_pressed;
        static EventEmitter<i32, i32, i32> s_on_key_released;
        static EventEmitter<GLFWwindow*> s_on_window_close;
    private:
        static std::vector<Window*> s_windows;
    };
}