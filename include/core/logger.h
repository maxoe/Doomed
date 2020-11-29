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
    static Logger* getRendererLogger();
    static Logger* getLoaderLogger();
    static Logger* getWorldLogger();

    void logError(const std::string& msg) const;
    void logInfo(const std::string& msg) const;
    void logWarning(const std::string& msg) const;

    static std::ostringstream* getUiLogStream();

    static void cleanUp();

private:
    Logger(const char* name);

    void initLogger(const char* name);
    static std::vector<spdlog::sink_ptr>* getLogSinks();

    inline static Logger* coreLogger = nullptr;
    inline static Logger* gameLogger = nullptr;
    inline static Logger* rendererLogger = nullptr;
    inline static Logger* loaderLogger = nullptr;
    inline static Logger* worldLogger = nullptr;

    inline static std::ostringstream* uiLogStream = nullptr;
    inline static std::vector<spdlog::sink_ptr>* logSinks = nullptr;
    std::unique_ptr<spdlog::logger> logger = nullptr;
};

// Core log macros
#define LOG_CORE_INFO(...) ::Logger::getCoreLogger()->logInfo(__VA_ARGS__)
#define LOG_CORE_WARN(...) ::Logger::getCoreLogger()->logWarning(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Logger::getCoreLogger()->logError(__VA_ARGS__)

// Game log macros
#define LOG_GAME_INFO(...) ::Logger::getGameLogger()->logInfo(__VA_ARGS__)
#define LOG_GAME_WARN(...) ::Logger::getGameLogger()->logWarning(__VA_ARGS__)
#define LOG_GAME_ERROR(...) ::Logger::getGameLogger()->logError(__VA_ARGS__)

// Renderer log macros
#define LOG_RENDERER_INFO(...) ::Logger::getRendererLogger()->logInfo(__VA_ARGS__)
#define LOG_RENDERER_WARN(...) ::Logger::getRendererLogger()->logWarning(__VA_ARGS__)
#define LOG_RENDERER_ERROR(...) ::Logger::getRendererLogger()->logError(__VA_ARGS__)

// Loader log macros
#define LOG_LOADER_INFO(...) ::Logger::getLoaderLogger()->logInfo(__VA_ARGS__)
#define LOG_LOADER_WARN(...) ::Logger::getLoaderLogger()->logWarning(__VA_ARGS__)
#define LOG_LOADER_ERROR(...) ::Logger::getLoaderLogger()->logError(__VA_ARGS__)

// World log macros
#define LOG_WORLD_INFO(...) ::Logger::getWorldLogger()->logInfo(__VA_ARGS__)
#define LOG_WORLD_WARN(...) ::Logger::getWorldLogger()->logWarning(__VA_ARGS__)
#define LOG_WORLD_ERROR(...) ::Logger::getWorldLogger()->logError(__VA_ARGS__)

#define LOG_CLEANUP Logger::cleanUp
