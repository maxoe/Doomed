#include "logging/logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <sstream>

Logger* Logger::coreLogger = 0;
Logger* Logger::gameLogger = 0;
Logger* Logger::shaderLogger = 0;
std::ostringstream* Logger::uiLogStream = 0;
std::vector<spdlog::sink_ptr>* Logger::logSinks = 0;

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
    std::vector<spdlog::sink_ptr>* tmp = Logger::getLogSinks();
    
    logger = std::make_unique<spdlog::logger>(name, begin(*tmp), end(*tmp));
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
}

std::vector<spdlog::sink_ptr>* Logger::getLogSinks()
{
    if (!logSinks)
    {
        logSinks = new std::vector<spdlog::sink_ptr>();
        auto terminal = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("Doomed.log", true);
        auto gui =
            std::make_shared<spdlog::sinks::ostream_sink_mt>(*Logger::getUiLogStream());
        (*logSinks) = {gui, file, terminal};

        (*logSinks)[0]->set_pattern("%^[%T] %n: %v%$");
        (*logSinks)[1]->set_pattern("[%T] [%l] %n: %v");
        (*logSinks)[2]->set_pattern("%^[%T] %n: %v%$");
    }
    return logSinks;
}

std::ostringstream* Logger::getUiLogStream()
{
    if (!uiLogStream)
    {
        uiLogStream = new std::ostringstream();
    }
    return uiLogStream;
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