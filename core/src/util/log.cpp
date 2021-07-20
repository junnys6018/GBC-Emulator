#include "util/log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace gbc
{
    Ref<spdlog::logger> Log::s_logger;

    void Log::initialize()
    {
        spdlog::set_pattern("%^[%T] [%n] [%l]: %v%$");
        s_logger = spdlog::stdout_color_mt("GBC");
        s_logger->set_level(spdlog::level::trace);
    }
}