#pragma once
#include <gbc.h>
#include <glad/glad.h>

#include "windowing/window.h"
#include "windowing/message_queue.h"
using namespace gbc;

namespace app
{
    class Application
    {
    public:
        static Application* start(int argc, char** argv);
        void run_gbc();
        void run_debug();
        ~Application();

    private:
        Application();
        void setup_event_listeners();

    private:
        Scope<GBC> m_gbc;
        Scope<Window> m_gbc_window;
        Scope<Window> m_debug_window;
        Scope<std::thread> m_gbc_thread;

        MessageQueue m_debug_queue;
        MessageQueue m_gbc_queue;

        std::string m_current_rom = "roms/Super Mario Land (World).gb";

        // imgui variables
        bool m_paused = true;
        i32 m_wait_addr = 0;

        static bool s_initialized;
    };
}