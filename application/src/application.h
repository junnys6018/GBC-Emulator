#pragma once
#include <gbc.h>
#include <glad/glad.h>

#include "windowing/window.h"
using namespace gbc;

namespace app
{
    class Application
    {
    public:
        static Application* start(int argc, char** argv);
        void run();
        ~Application();

    private:
        Application();
        void draw_cpu_window();

    private:
        Scope<GBC> m_gbc;
        Scope<Window> m_window;

        // IMGUI variables
        bool m_paused = true;
        u32 m_step_count = 0;
        i32 m_wait_addr = 0;

        static bool s_initialized;
    };
}