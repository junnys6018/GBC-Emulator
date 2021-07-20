#include "application.h"

#include <gbc.h>

#include <glad/glad.h>

#include "windowing/glfw_init.h"
#include <GLFW/glfw3.h>

#include "windowing/imgui_init.h"
#include <imgui.h>

namespace app
{
    bool Application::s_initialized = false;

    Application* Application::start(int, char**)
    {
        if (!s_initialized)
        {
            s_initialized = true;
            return new Application();
        }
        else
        {
            LOG_ERROR("Attempt to call Application::start() more than once");
            exit(EXIT_FAILURE);
        }
    }

    Application::Application()
    {
        gbc::initialize();
        LOG_INFO("{}", gbc::hello_world());

        GLFWwindow* window = Glfw::initialize();
        ImGuiLayer::initialize(window);
    }

    Application::~Application()
    {
        ImGuiLayer::shutdown();
        Glfw::shutdown();
    }

    void Application::run()
    {
        // Game loop
        while (!glfwWindowShouldClose(Glfw::s_window))
        {
            Glfw::begin();
            ImGuiLayer::begin();

            // Rendering

            // Clear the colorbuffer
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui::ShowDemoWindow();

            ImGuiLayer::end();
            Glfw::end();
        }
    }
}