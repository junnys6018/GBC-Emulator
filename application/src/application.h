#pragma once

namespace app
{
    class Application
    {
    public:
        static Application* Start(int argc, char** argv);
        void Run();
        ~Application();

    private:
        Application();
        static bool s_initialized;
    };
}