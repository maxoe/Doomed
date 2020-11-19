#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

class Logger
{
public:
    static Logger* getCoreLogger();
    static Logger* getGameLogger();
    static Logger* getShaderLogger();

    void logError(std::string msg);
    void logInfo(std::string msg);
    void logWarning(std::string msg);

    static std::ostringstream* getUiLogStream();

private:
    static Logger* coreLogger;
    static Logger* gameLogger;
    static Logger* shaderLogger;
    static std::ostringstream* uiLogStream;
    static std::vector<spdlog::sink_ptr>* logSinks;

    std::unique_ptr<spdlog::logger> logger = nullptr;

    Logger(const char* name);

    static std::vector<spdlog::sink_ptr>* getLogSinks();
    
    void initLogger(const char* name);
};

// Core log macros
#define LOG_CORE_INFO(...) ::Logger::getCoreLogger()->logInfo(__VA_ARGS__)
#define LOG_CORE_WARN(...) ::Logger::getCoreLogger()->logWarning(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Logger::getCoreLogger()->logError(__VA_ARGS__)

// Game log macros
#define LOG_GAME_INFO(...) ::Logger::getGameLogger()->logInfo(__VA_ARGS__)
#define LOG_GAME_WARN(...) ::Logger::getGameLogger()->logWarning(__VA_ARGS__)
#define LOG_GAME_ERROR(...) ::Logger::getGameLogger()->logError(__VA_ARGS__)

// Shader log macros
#define LOG_SHADER_INFO(...) ::Logger::getShaderLogger()->logInfo(__VA_ARGS__)
#define LOG_SHADER_WARN(...) ::Logger::getShaderLogger()->logWarning(__VA_ARGS__)
#define LOG_SHADER_ERROR(...) ::Logger::getShaderLogger()->logError(__VA_ARGS__)

