#include "util/log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace gbc
{
    Ref<spdlog::logger> Log::s_core_logger;
    Ref<spdlog::logger> Log::s_client_logger;

    void Log::initialize(spdlog::level::level_enum log_level)
    {
        spdlog::set_pattern("%^[%T] [%n] [%l]: %v%$");
        s_core_logger = spdlog::stdout_color_mt("GBC");
        s_core_logger->set_level(log_level);

        s_client_logger = spdlog::stderr_color_mt("CLIENT");
        s_client_logger->set_level(spdlog::level::trace);
    }
}