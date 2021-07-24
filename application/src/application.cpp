#include "application.h"

#include <gbc.h>

#include "windowing/glfw_init.h"
#include <GLFW/glfw3.h>

#include "windowing/imgui_init.h"
#include <imgui.h>

#include "debug/disassembly.h"
#include "debug/memory.h"
#include "debug/stack.h"

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

        m_window = create_scope<Window>("GBC Emulator - By Jun Lim", 1550, 870, true);
        ImGuiLayer::initialize(m_window->m_handle);

        m_gbc = create_scope<GBC>("roms/04-op r,imm.gb");
        
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
        static MemoryWindow memory_window;
        static StackWindow stack_window;
        u32 cnt = 0;
        i32 inc = cnt / 144;
        // Game loop
        while (!glfwWindowShouldClose(m_window->m_handle))
        {
            Glfw::poll_events();
            ImGuiLayer::begin();
            ImGui::DockSpaceOverViewport();

            // Rendering
            draw_cpu_window();
            disassembly_window.draw_window("Disassembly", *m_gbc, m_gbc->get_pc());
            memory_window.draw_window("Memory", *m_gbc);
            stack_window.draw_window("Stack", *m_gbc);

            if (m_step_count < cnt)
            {
                if (m_step_count + inc > cnt)
                {
                    i32 num = cnt - m_step_count;
                    for (int i = 0; i < num; i++)
                        m_gbc->step();
                    m_step_count += num;
                }
                else
                {
                    for (int i = 0; i < inc; i++)
                        m_gbc->step();
                    m_step_count += inc;
                }
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
        static const char* flags[8] = {".", ".", ".", ".", "c", "h", "n", "z"};
        static const ImVec4 green = {0, 1, 0, 1};
        static const ImVec4 red = {1, 0, 0, 1};

        ImGui::Begin("CPU");
        CPUData d = m_gbc->get_cpu_data();
        ImGui::Text("AF: $%04X", d.AF);
        ImGui::SameLine();
        for (i32 i = 7; i >= 0; i--)
        {
            bool active = ((lsb(d.AF) & (1 << i)) > 0);
            ImGui::TextColored(active ? green : red, flags[i]);
            if (i != 0)
                ImGui::SameLine(0, 0);
        }
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

        static char addr_input_buf[64];
        if (ImGui::InputText("goto", addr_input_buf, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            sscanf(addr_input_buf, "%X", &m_wait_addr);
            u16 addr = static_cast<u16>(m_wait_addr);
            while (m_gbc->get_pc() != addr)
            {
                m_step_count++;
                m_gbc->step();
            }
        }
        ImGui::End();
    }
}