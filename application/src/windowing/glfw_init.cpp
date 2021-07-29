#include "glfw_init.h"
#include <util/log.h>

static void glfw_error_callback(int error, const char* description) { LOG_ERROR("glfw error {}: {}", error, description); }

namespace app
{

    void Glfw::initialize()
    {
        LOG_INFO("Initializing GLFW context");

        // Init GLFW
        glfwSetErrorCallback(glfw_error_callback);
        glfwInit();
        // Set all the required options for GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    void Glfw::shutdown()
    {
        // Terminates GLFW, clearing any resources allocated by GLFW.
        LOG_INFO("Shutting down GLFW context");
        glfwTerminate();
    }

    void Glfw::poll_events()
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
    }

}