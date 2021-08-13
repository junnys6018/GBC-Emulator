#include "application.h"

#include <gbc.h>

#include "windowing/glfw_init.h"
#include <GLFW/glfw3.h>

#include "windowing/imgui_init.h"
#include <imgui.h>

#include "debug/disassembly.h"
#include "debug/io_registers.h"
#include "debug/lcd.h"
#include "debug/memory.h"
#include "debug/stack.h"
#include "debug/tiledata.h"
#include "debug/tilemap.h"

#include "opengl/debug.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <chrono>

// const char* rom = "roms/Super Mario Land (World).gb";
const char* rom = "../tests/roms/mooneye-gb_hwtests/acceptance/timer/tima_write_reloading.gb";

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
            CLIENT_LOG_ERROR("Attempt to call Application::start() more than once");
            exit(EXIT_FAILURE);
        }
    }

    Application::Application()
    {
        gbc::initialize(spdlog::level::info);
        Glfw::initialize();

        m_window = create_scope<Window>("GBC Emulator - By Jun Lim", 1550, 870, true);
        ImGuiLayer::initialize(m_window->m_handle);

        m_gbc = create_scope<GBC>(rom);

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
        static TiledataWindow tiledata_window;
        static TilemapWindow tilemap_window;
        static IORegistersWindow registers_window;
        static LCDWindow lcd_window;
        static std::chrono::steady_clock clock;

        u32 cnt = 0;
        i32 inc = cnt / 144;
        // Game loop

        auto beg = clock.now();
        while (!glfwWindowShouldClose(m_window->m_handle))
        {
            Glfw::poll_events();
            ImGuiLayer::begin();
            ImGui::DockSpaceOverViewport();

            // Set the keys
            Input& input = m_window->m_input;
            gbc::Keys keys;
            keys.up = !input.is_key_down(GLFW_KEY_UP);
            keys.down = !input.is_key_down(GLFW_KEY_DOWN);
            keys.left = !input.is_key_down(GLFW_KEY_LEFT);
            keys.right = !input.is_key_down(GLFW_KEY_RIGHT);
            keys.a = !input.is_key_down(GLFW_KEY_X);
            keys.b = !input.is_key_down(GLFW_KEY_Z);
            keys.start = !input.is_key_down(GLFW_KEY_ENTER);
            keys.select = !input.is_key_down(GLFW_KEY_TAB);
            m_gbc->set_keys(keys);

            // Rendering
            draw_cpu_window();
            disassembly_window.draw_window("Disassembly", *m_gbc, m_gbc->get_pc());
            memory_window.draw_window("Memory", *m_gbc);
            stack_window.draw_window("Stack", *m_gbc);
            tiledata_window.draw_window("Tiledata", *m_gbc);
            registers_window.draw_window("IO Registers", *m_gbc);
            tilemap_window.draw_window("Tilemap", *m_gbc);
            lcd_window.draw_window("LCD", *m_gbc);

            if (m_step_count < cnt)
            {
                if (m_step_count + inc > cnt)
                {
                    i32 num = cnt - m_step_count;
                    for (int i = 0; i < num; i++)
                    {
                        m_gbc->step();
                    }
                    m_step_count += num;
                }
                else
                {
                    for (int i = 0; i < inc; i++)
                    {
                        m_gbc->step();
                    }
                    m_step_count += inc;
                }
            }

            auto tp = clock.now();
            using my_duration = std::chrono::duration<double, std::ratio<1, 1>>;
            if (!m_paused)
            {
                my_duration duration = std::chrono::duration_cast<my_duration>(tp - beg);
                i32 clocks = duration.count() * gbc::MASTER_CLOCK_FREQ;
                for (int i = 0; i < clocks; i++)
                {
                    m_gbc->clock();
                }
            }
            beg = tp;

            // Clear the colorbuffer
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
        ImGui::Text("IME: %i", d.IME);
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
                m_gbc->clock();
            }
        }

        if (ImGui::Button("reset"))
        {
            m_paused = true;
            m_step_count = 0;
            m_wait_addr = 0;
            m_gbc = create_scope<GBC>(rom);
        }

#ifdef PLATFORM_WINDOWS
        if (ImGui::Button("open..."))
        {
            char filepath[256];
            OPENFILENAMEA ofn;

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFile = filepath;
            // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
            // use the contents of szFile to initialize itself.
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = 256;
            ofn.lpstrFilter = "All\0*.*\0GB (.gb)\0*.gb\0";
            ofn.nFilterIndex = 2;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

            if (GetOpenFileNameA(&ofn))
            {
                m_paused = true;
                m_step_count = 0;
                m_wait_addr = 0;
                m_gbc = create_scope<GBC>(filepath);
            }
        }
#endif

        if (ImGui::Button("dump bg tilemap"))
        {
            auto tilemap = m_gbc->dump_bg_tile_map();
            write_file("tilemap.bin", tilemap);
        }
        ImGui::End();
    }
}