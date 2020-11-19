#include "logging/logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <iostream>

Logger* Logger::coreLogger = 0;
Logger* Logger::gameLogger = 0;
Logger* Logger::shaderLogger = 0;

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

Logger* Logger::getShaderLogger()
{
    if (!shaderLogger)
    {
        shaderLogger = new Logger("SHADER");
    }
    return shaderLogger;
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

void Logger::logError(std::string msg)
{
    logger->error(std::string("[ERROR] ") + msg);
}

void Logger::logInfo(std::string msg)
{
    logger->info(std::string("[INFO] ") + msg);
}

void Logger::logWarning(std::string msg)
{
    logger->warn(std::string("[WARNING] ") + msg);
}
