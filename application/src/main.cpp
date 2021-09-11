#include "application.h"

int main(int argc, char** argv)
{
    using namespace app;

    Application* app = Application::start(argc, argv);
    app->run_debug(); // debug window is run on main thread
    delete app;

    return 0;
}