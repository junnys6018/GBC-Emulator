#include "glfw_init.h"
#include <util/log.h>

static void glfw_error_callback(int error, const char* description) { CLIENT_LOG_ERROR("glfw error {}: {}", error, description); }

namespace app
{

    void Glfw::initialize()
    {
        CLIENT_LOG_INFO("Initializing GLFW context");

        // Init GLFW
        glfwSetErrorCallback(glfw_error_callback);
        if (glfwInit() == GLFW_FALSE)
        {
            CLIENT_LOG_ERROR("Failed to initialize glfw");
            exit(EXIT_FAILURE);
        }
    }

    void Glfw::shutdown()
    {
        // Terminates GLFW, clearing any resources allocated by GLFW.
        CLIENT_LOG_INFO("Shutting down GLFW context");
        glfwTerminate();
    }

    void Glfw::poll_events()
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
    }

}