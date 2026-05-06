#ifndef CORE_LOG_H
#define CORE_LOG_H

#include <cstdint>


namespace engine {

enum class LogLevel : uint8_t {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO  = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_ERROR = 3
};

void LogSetLevel(LogLevel level);
LogLevel LogGetLevel();

void LogWrite(LogLevel level, const char* module, const char* fmt, ...);

#define LOG_DEBUG(mod, ...) \
    LogWrite(LogLevel::LOG_LEVEL_DEBUG, mod, __VA_ARGS__)
#define LOG_INFO(mod, ...) \
    LogWrite(LogLevel::LOG_LEVEL_INFO, mod, __VA_ARGS__)
#define LOG_WARN(mod, ...) \
    LogWrite(LogLevel::LOG_LEVEL_WARN, mod, __VA_ARGS__)
#define LOG_ERROR(mod, ...) \
    LogWrite(LogLevel::LOG_LEVEL_ERROR, mod, __VA_ARGS__)


} /* namespace engine */
#endif
