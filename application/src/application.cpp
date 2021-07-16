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

    Application* Application::Start(int, char**)
    {
        if (!s_initialized)
        {
            s_initialized = true;
            return new Application();
        }
        else
        {
            LOG_ERROR("Attempt to call Application::Start() more than once");
            exit(EXIT_FAILURE);
        }
    }

    Application::Application()
    {
        gbc::Initialize();
        LOG_INFO("{}", gbc::HelloWorld());

        GLFWwindow* window = Glfw::Initialize();
        ImGuiLayer::Initialize(window);
    }

    Application::~Application()
    {
        ImGuiLayer::Shutdown();
        Glfw::Shutdown();
    }

    void Application::Run()
    {
        // Game loop
        while (!glfwWindowShouldClose(Glfw::s_window))
        {
            Glfw::Begin();
            ImGuiLayer::Begin();

            // Rendering

            // Clear the colorbuffer
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui::ShowDemoWindow();

            ImGuiLayer::End();
            Glfw::End();
        }
    }
}