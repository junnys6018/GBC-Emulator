#define CATCH_CONFIG_RUNNER
#include <catch_amalgamated.hpp>
#include <gbc.h>

#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX)
#error Unknown platform
#endif

#if !defined(CONFIGURATION_DEBUG) && !defined(CONFIGURATION_RELEASE)
#error Unknown configuration
#endif

#if defined(PLATFORM_WINDOWS)
static const char* platform_string = "Windows";
#elif defined(PLATFORM_LINUX)
static const char* platform_string = "Linux";
#endif

#if defined(CONFIGURATION_DEBUG)
static const char* configuration_string = "Debug";
#elif defined(CONFIGURATION_RELEASE)
static const char* configuration_string = "Release";
#endif

int main(int argc, char* argv[])
{
    gbc::initialize(spdlog::level::err, true);
    CLIENT_LOG_INFO("Running GBC tests ({}, {})", platform_string, configuration_string);

    int result = Catch::Session().run(argc, argv);
    return result;
}