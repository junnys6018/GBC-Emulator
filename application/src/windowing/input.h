#pragma once
#include "common.h"
#include "windowing/event_emitter.h"
#include <GLFW/glfw3.h>

namespace app
{
    class Input
    {
    public:
        bool is_key_down(i32 key);

    public:
        EventEmitter<i32, i32, i32> m_on_key_pressed;
        EventEmitter<i32, i32, i32> m_on_key_released;
        EventEmitter<GLFWwindow*> m_on_window_close;

    private:
        GLFWwindow* m_window = nullptr;
        void set_window(GLFWwindow* window);

        friend class Window;
    };
}
