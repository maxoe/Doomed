#include "core/logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

Logger* Logger::getCoreLogger()
{
    if (!coreLogger)
    {
        coreLogger = new Logger("CORE");
    }
    return coreLogger;
};

Logger* Logger::getGameLogger()
{
    if (!gameLogger)
    {
        gameLogger = new Logger("GAME");
    }
    return gameLogger;
};

Logger* Logger::getRendererLogger()
{
    if (!rendererLogger)
    {
        rendererLogger = new Logger("SHADER");
    }
    return rendererLogger;
};

Logger* Logger::getLoaderLogger()
{
    if (!rendererLogger)
    {
        rendererLogger = new Logger("LOADER");
    }
    return rendererLogger;
};

Logger::Logger(const char* name)
{
    initLogger(name);
}

void Logger::initLogger(const char* name)
{
    std::vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Doomed.log", true));

    logSinks[0]->set_pattern("%^[%T] %n: %v%$");
    logSinks[1]->set_pattern("[%T] [%l] %n: %v");

    logger = std::make_unique<spdlog::logger>(name, begin(logSinks), end(logSinks));
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
}

void Logger::logError(const std::string& msg) const
{
    logger->error(std::string("[ERROR] ") + msg);
}

void Logger::logInfo(const std::string& msg) const
{
    logger->info(std::string("[INFO] ") + msg);
}

void Logger::logWarning(const std::string& msg) const
{
    logger->warn(std::string("[WARNING] ") + msg);
}

void Logger::cleanUp()
{
    delete coreLogger;
    delete gameLogger;
    delete rendererLogger;
    delete loaderLogger;
}
