#pragma once
#include <gbc.h>
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
        void draw_disassembly();

    private:
        Scope<GBC> m_gbc;
        bool m_paused = true;
        u32 m_step_count = 0;

        static bool s_initialized;
    };
}