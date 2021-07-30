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
        static void initialize(spdlog::level::level_enum log_level);

        static Ref<spdlog::logger>& get_logger() { return s_logger; }

        static inline void flush() { s_logger->flush(); }
        static inline void set_level(spdlog::level::level_enum log_level) { s_logger->set_level(log_level); }

    private:
        static Ref<spdlog::logger> s_logger;
    };
}

// log macros
#define LOG_TRACE(...) ::gbc::Log::get_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::gbc::Log::get_logger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::gbc::Log::get_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::gbc::Log::get_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::gbc::Log::get_logger()->critical(__VA_ARGS__)