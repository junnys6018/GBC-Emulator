#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>

#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include "util/common.h"

namespace gbc
{
    enum class LoggerType
    {
        CORE,
        CLIENT
    };

    class Log
    {
    public:
        static void initialize(spdlog::level::level_enum log_level);

        static Ref<spdlog::logger>& get_core_logger() { return s_core_logger; }
        static Ref<spdlog::logger>& get_client_logger() { return s_client_logger; }

        static inline void flush()
        {
            s_core_logger->flush();
            s_client_logger->flush();
        }
        static inline void set_level(spdlog::level::level_enum log_level, LoggerType type)
        {
            switch (type)
            {
            case LoggerType::CORE: s_core_logger->set_level(log_level); break;
            case LoggerType::CLIENT: s_client_logger->set_level(log_level); break;
            }
        }

    private:
        static Ref<spdlog::logger> s_core_logger;
        static Ref<spdlog::logger> s_client_logger;
    };
}

// log macros
#define CORE_LOG_TRACE(...) ::gbc::Log::get_core_logger()->trace(__VA_ARGS__)
#define CORE_LOG_INFO(...) ::gbc::Log::get_core_logger()->info(__VA_ARGS__)
#define CORE_LOG_WARN(...) ::gbc::Log::get_core_logger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...) ::gbc::Log::get_core_logger()->error(__VA_ARGS__)
#define CORE_LOG_CRITICAL(...) ::gbc::Log::get_core_logger()->critical(__VA_ARGS__)

#define CLIENT_LOG_TRACE(...) ::gbc::Log::get_client_logger()->trace(__VA_ARGS__)
#define CLIENT_LOG_INFO(...) ::gbc::Log::get_client_logger()->info(__VA_ARGS__)
#define CLIENT_LOG_WARN(...) ::gbc::Log::get_client_logger()->warn(__VA_ARGS__)
#define CLIENT_LOG_ERROR(...) ::gbc::Log::get_client_logger()->error(__VA_ARGS__)
#define CLIENT_LOG_CRITICAL(...) ::gbc::Log::get_client_logger()->critical(__VA_ARGS__)