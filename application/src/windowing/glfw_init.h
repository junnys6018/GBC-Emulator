#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "common.h"

namespace app
{
    class Glfw
    {
    public:
        static void initialize();
        static void shutdown();
        static void poll_events();
    };
}