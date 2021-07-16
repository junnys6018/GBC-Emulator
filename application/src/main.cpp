#include "application.h"

int main(int argc, char** argv)
{
    using namespace app;

    Application* app = Application::Start(argc, argv);
    app->Run();
    delete app;

    return 0;
}