#include "application.h"

int main(int argc, char** argv)
{
    using namespace app;

    Application* app = Application::start(argc, argv);
    app->run();
    delete app;

    return 0;
}