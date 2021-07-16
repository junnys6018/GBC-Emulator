#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>

#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include "util/common.h"

namespace gbc
{
    class Log
    {
    public:
        static void Initialize();

        static Ref<spdlog::logger> GetLogger()
        {
            return s_logger;
        }

    private:
        static Ref<spdlog::logger> s_logger;
    };
}

// log macros
#define LOG_TRACE(...) ::gbc::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::gbc::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::gbc::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::gbc::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::gbc::Log::GetLogger()->critical(__VA_ARGS__)