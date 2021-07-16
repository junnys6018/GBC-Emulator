#include "glfw_init.h"
#include <util/log.h>

// Window dimensions
static const GLuint WIDTH = 800, HEIGHT = 600;

static void glfw_error_callback(int error, const char* description)
{
    LOG_ERROR("glfw error {}: {}", error, description);
}

namespace app
{
    GLFWwindow* Glfw::s_window = NULL;

    GLFWwindow* Glfw::Initialize()
    {
        LOG_INFO("Initializing GLFW context");

        // Init GLFW
        glfwSetErrorCallback(glfw_error_callback);
        glfwInit();
        // Set all the required options for GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        // Create a window
        s_window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
        glfwMakeContextCurrent(s_window);
        glfwSwapInterval(1);
        if (s_window == NULL)
        {
            LOG_ERROR("Failed to create GLFW window");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG_ERROR("Failed to initialize OpenGL context");
            exit(EXIT_FAILURE);
        }

        // Define the viewport dimensions
        glViewport(0, 0, WIDTH, HEIGHT);

        return s_window;
    }

    void Glfw::Shutdown()
    {
        // Terminates GLFW, clearing any resources allocated by GLFW.
        LOG_INFO("Shutting down GLFW context");
        glfwTerminate();
    }

    void Glfw::Begin()
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
    }

    void Glfw::End()
    {
        // Swap the screen buffers
        glfwSwapBuffers(s_window);
    }
}