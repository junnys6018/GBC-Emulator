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

    private:
        Scope<GBC> m_gbc;
        static bool s_initialized;
    };
}