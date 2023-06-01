#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cubos/core/log.hpp>

void cubos::core::initializeLogger()
{
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::trace);
    consoleSink->set_pattern("%^[cubos] [%s:%# %!] %l: %v%$");

    // Only print to the file warnings, errors and critical logs
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log/cubos.txt", true);
    fileSink->set_level(spdlog::level::warn);
    fileSink->set_pattern("[cubos] [%s:%# %!] %l: %v");

    auto logger = std::make_shared<spdlog::logger>("cubos", spdlog::sinks_init_list({consoleSink, fileSink}));
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
    spdlog::set_default_logger(logger);
}

void cubos::core::disableLogging()
{
    spdlog::set_level(spdlog::level::critical);
}
