#include "application.h"

#include <gbc.h>

#include <glad/glad.h>

#include "windowing/glfw_init.h"
#include <GLFW/glfw3.h>

#include "windowing/imgui_init.h"
#include <imgui.h>

#include <imgui_memory_editor.h>

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

        m_gbc = create_scope<GBC>();
    }

    Application::~Application()
    {
        ImGuiLayer::shutdown();
        Glfw::shutdown();
    }

    void Application::run()
    {
        static MemoryEditor memory_view;
        memory_view.ReadOnly = true;
        memory_view.OptShowAscii = false;
        u8* memory = m_gbc->get_memory();

        // Game loop
        while (!glfwWindowShouldClose(Glfw::s_window))
        {
            Glfw::begin();
            ImGuiLayer::begin();
            ImGui::DockSpaceOverViewport();

            // Rendering
            draw_cpu_window();
            memory_view.DrawWindow("Memory Editor", m_gbc->get_memory(), 65536);

            m_gbc->step();

            // Clear the colorbuffer
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui::ShowDemoWindow();

            ImGuiLayer::end();
            Glfw::end();
        }
    }

    void Application::draw_cpu_window()
    {
        ImGui::Begin("CPU");
        CPUData d = m_gbc->get_cpu_data();
        ImGui::Text("AF: $%04X", d.AF);
        ImGui::Text("BC: $%04X", d.BC);
        ImGui::Text("DE: $%04X", d.DE);
        ImGui::Text("HL: $%04X", d.HL);
        ImGui::Text("SP: $%04X", d.SP);
        ImGui::Text("PC: $%04X", d.PC);
        ImGui::Text("Total Machine Cycles: %u", d.total_machine_cycles);
        if (ImGui::Button("step"))
        {
            m_gbc->step();
        }
        ImGui::End();
    }
}