#include "application.h"

#include <gbc.h>

#include "windowing/glfw_init.h"
#include <GLFW/glfw3.h>

#include "windowing/imgui_init.h"
#include <imgui.h>

#include "windowing/window_manager.h"
#include "lcd_renderer.h"

#include "debug/cpu.h"
#include "debug/disassembly.h"
#include "debug/io_registers.h"
#include "debug/lcd.h"
#include "debug/memory.h"
#include "debug/stack.h"
#include "debug/tiledata.h"
#include "debug/tilemap.h"

#include "opengl/debug.h"

#include <chrono>

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
        m_gbc = create_scope<GBC>(m_current_rom);
        Glfw::initialize();

        // Create the debug window, this will run on the main thread
        m_debug_window = create_scope<Window>("Debug Window", 1550, 870, true);
        ImGuiLayer::initialize(m_debug_window->m_handle);

        // Create the gbc window, this will run on a worker thread
        m_gbc_window = create_scope<Window>("GBC Emulator - By Jun Lim", 320, 288, true);
        m_gbc_thread = create_scope<std::thread>(std::bind(&Application::run_gbc, this));

        setup_event_listeners();
    }

    void Application::setup_event_listeners()
    {
        m_debug_window->m_input.m_on_key_pressed.add_event_listener([&](i32 key, i32 scancode, i32 mods) -> bool {
            if (key == GLFW_KEY_SPACE)
            {
                m_gbc_queue.push_back(Message(MessageType::STEP_EMULATION));
                return true;
            }
            return false;
        });

        m_debug_window->m_input.m_on_window_close.add_event_listener([](GLFWwindow* window) -> bool {
            glfwSetWindowShouldClose(window, GLFW_FALSE);
            glfwHideWindow(window);
            return false;
        });

        WindowManager::s_on_key_pressed.add_event_listener([&](i32 key, i32 scancode, i32 mods) -> bool {
            if (key == GLFW_KEY_D && (mods & GLFW_MOD_CONTROL))
            {
                GLFWwindow* window = m_debug_window->m_handle;
                bool visible = glfwGetWindowAttrib(window, GLFW_VISIBLE);
                if (!visible)
                    glfwShowWindow(window);
                else
                    glfwHideWindow(window);

                return true;
            }
            return false;
        });
    }

    Application::~Application()
    {
        m_gbc_window.reset();
        ImGuiLayer::shutdown();
        m_debug_window.reset();
        Glfw::shutdown();
    }

    void Application::run_gbc()
    {
        glfwMakeContextCurrent(m_gbc_window->m_handle);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        static std::chrono::steady_clock clock;
        static LCDRenderer lcd_renderer;
        u32 cycle = 0;
        i32 inc = cycle / 144;
        // Game loop

        auto beg = clock.now();

        while (!glfwWindowShouldClose(m_gbc_window->m_handle))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            Message message;
            while (m_gbc_queue.pop_front(message))
            {
                switch (message.m_type)
                {
                case MessageType::STEP_EMULATION: m_gbc->step(); break;
                case MessageType::TOGGLE_PAUSE: m_paused = !m_paused; break;
                case MessageType::RESET_GBC:
                    m_gbc = create_scope<GBC>(m_current_rom);
                    m_paused = true;
                    break;
                }
            }

            // Set the keys, the gbc reads keys inverted
            // 0: pressed; 1: released
            Input& input = m_gbc_window->m_input;
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

            if (m_gbc->get_m_cycles() < cycle)
            {
                if (m_gbc->get_m_cycles() + inc > cycle)
                {
                    i32 num = cycle - m_gbc->get_m_cycles();
                    for (int i = 0; i < num; i++)
                    {
                        m_gbc->clock();
                    }
                }
                else
                {
                    for (int i = 0; i < inc; i++)
                    {
                        m_gbc->clock();
                    }
                }
            }

            // Draw the screen
            lcd_renderer.render(m_gbc->get_framebuffer());

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

            m_gbc_window->swap_buffers();
        }
        m_debug_queue.push_back(Message(MessageType::GBC_WINDOW_SHUTDOWN));
    }

    void Application::run_debug()
    {
        glfwMakeContextCurrent(m_debug_window->m_handle);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        static DisassemblyWindow disassembly_window;
        static MemoryWindow memory_window;
        static StackWindow stack_window;
        static TiledataWindow tiledata_window;
        static TilemapWindow tilemap_window;
        static IORegistersWindow registers_window;
        static LCDWindow lcd_window;
        static CPUWindow cpu_window;

        while (true)
        {
            Glfw::poll_events();
            glClear(GL_COLOR_BUFFER_BIT);

            Message message;
            while (m_debug_queue.pop_front(message))
            {
                switch (message.m_type)
                {
                case MessageType::GBC_WINDOW_SHUTDOWN: m_gbc_thread->join(); return;
                }
            }

            ImGuiLayer::begin();
            ImGui::DockSpaceOverViewport();

            // Rendering
            disassembly_window.draw_window("Disassembly", *m_gbc, m_gbc->get_pc());
            memory_window.draw_window("Memory", *m_gbc);
            stack_window.draw_window("Stack", *m_gbc);
            tiledata_window.draw_window("Tiledata", *m_gbc);
            registers_window.draw_window("IO Registers", *m_gbc);
            tilemap_window.draw_window("Tilemap", *m_gbc);
            lcd_window.draw_window("LCD", *m_gbc);
            cpu_window.draw_window("CPU", *m_gbc, m_gbc_queue);

            ImGui::ShowDemoWindow();

            ImGuiLayer::end();

            m_debug_window->swap_buffers();
        }
    }
}