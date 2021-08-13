#pragma once

#if defined(CONFIGURATION_DEBUG)

#if defined(PLATFORM_WINDOWS)
#define DEBUGBREAK() __debugbreak()
#elif defined(PLATFORM_LINUX)
#include <signal.h>
#define DEBUGBREAK() raise(SIGTRAP)
#endif

#define ASSERT(x)                                                                                                                          \
    {                                                                                                                                      \
        if (!(x))                                                                                                                          \
        {                                                                                                                                  \
            DEBUGBREAK();                                                                                                                  \
        }                                                                                                                                  \
    }

#else
#define ASSERT(x) (void)0
#endif