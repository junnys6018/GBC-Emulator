#include "input.h"
#include "common.h"
#include "util/assert.h"
#include "windowing/window_manager.h"

namespace app
{
    void Input::set_window(GLFWwindow* window)
    {
        ASSERT(m_window == nullptr); // Window cannot be already set
        m_window = window;
        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Input& input = WindowManager::get_input(window);
            switch (action)
            {
            case GLFW_REPEAT:
            case GLFW_PRESS: input.m_on_key_pressed.trigger(key); break;
            case GLFW_RELEASE: input.m_on_key_released.trigger(key); break;
            }
        });
    }

    bool Input::is_key_down(i32 key)
    {
        int state = glfwGetKey(m_window, key);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

}
