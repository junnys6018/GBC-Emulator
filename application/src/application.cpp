#include "application.h"

#include <gbc.h>

#include "windowing/glfw_init.h"
#include <GLFW/glfw3.h>

#include "windowing/imgui_init.h"
#include <imgui.h>

#include <imgui_memory_editor.h>

#include "debug/disassembly.h"

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
        Glfw::initialize();

        m_window = create_scope<Window>("GBC Emulator - By Jun Lim", 1080, 720, true);
        ImGuiLayer::initialize(m_window->m_handle);

        m_gbc = create_scope<GBC>("roms/test.gb");

        m_window->m_input.m_on_key_pressed.add_event_listener([&](i32 key) -> bool {
            if (key == GLFW_KEY_SPACE)
            {
                m_gbc->step();
                m_step_count++;
                return true;
            }
            return false;
        });
    }

    Application::~Application()
    {
        ImGuiLayer::shutdown();
        Glfw::shutdown();
    }

    void Application::run()
    {
        static DisassemblyWindow disassembly_window;
        u32 cnt = 500;
        // Game loop
        while (!glfwWindowShouldClose(m_window->m_handle))
        {
            Glfw::poll_events();
            ImGuiLayer::begin();
            ImGui::DockSpaceOverViewport();

            // Rendering
            draw_cpu_window();
            disassembly_window.draw_window("Disassembly", *m_gbc, m_gbc->get_pc());

            if (m_step_count < cnt)
            {
                m_gbc->step();
                m_step_count++;
            }

            if (!m_paused)
            {
                m_gbc->step();
                m_step_count++;
            }

            // Clear the colorbuffer
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui::ShowDemoWindow();

            ImGuiLayer::end();
            m_window->swap_buffers();
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
            m_step_count++;
        }
        ImGui::SameLine();
        ImGui::Text("Count: %i", m_step_count);
        ImGui::SameLine();
        ImGui::Checkbox("Pause", &m_paused);
        ImGui::End();
    }
    void Application::draw_disassembly() {}
}