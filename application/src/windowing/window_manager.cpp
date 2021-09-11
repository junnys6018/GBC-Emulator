#include "windowing/window_manager.h"
#include "util/assert.h"
#include "windowing/window.h"

namespace app
{
    std::vector<Window*> WindowManager::s_windows;
    EventEmitter<i32, i32, i32> WindowManager::s_on_key_pressed;
    EventEmitter<i32, i32, i32> WindowManager::s_on_key_released;
    EventEmitter<GLFWwindow*> WindowManager::s_on_window_close;

    void WindowManager::add(Window* window)
    {
        ASSERT(std::find(s_windows.begin(), s_windows.end(), window) == s_windows.end());
        s_windows.push_back(window);
    }
    void WindowManager::remove(Window* window)
    {
        auto it = std::find(s_windows.begin(), s_windows.end(), window);
        ASSERT(it != s_windows.end());
        s_windows.erase(it);
    }
    Input& WindowManager::get_input(GLFWwindow* glfw_window)
    {
        auto it = std::find_if(s_windows.begin(), s_windows.end(),
                               [glfw_window](const Window* window) -> bool { return window->m_handle == glfw_window; });
        ASSERT(it != s_windows.end());
        return (*it)->m_input;
    }
}