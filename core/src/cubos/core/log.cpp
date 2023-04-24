#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cubos/core/log.hpp>

void cubos::core::initializeLogger()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("%^[cubos] [%s:%# %!] %l: %v%$");

    // Only print to the file warnings, errors and critical logs
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log/cubos.txt", true);
    file_sink->set_level(spdlog::level::warn);
    file_sink->set_pattern("[cubos] [%s:%# %!] %l: %v");

    auto logger = std::make_shared<spdlog::logger>("cubos", spdlog::sinks_init_list({console_sink, file_sink}));
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
    spdlog::set_default_logger(logger);
}
