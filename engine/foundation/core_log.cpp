#include "core_log.h"
#include <cstdio>
#include <ctime>
#include <stdarg.h>
#include <windows.h>
using namespace engine;
static LogLevel g_logLevel = LogLevel::LOG_LEVEL_DEBUG;

void LogSetLevel(LogLevel level)
{
    g_logLevel = level;
}

LogLevel LogGetLevel()
{
    return g_logLevel;
}

static void PrintWithConsoleEncoding(const char* utf8Str)
{
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);
    if (wideLen > 0) {
        wchar_t* wideBuf = (wchar_t*)_alloca(wideLen * sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wideBuf, wideLen);

        UINT consoleCP = GetConsoleOutputCP();
        int outLen = WideCharToMultiByte(consoleCP, 0, wideBuf, -1, nullptr, 0, nullptr, nullptr);
        if (outLen > 0) {
            char* outBuf = (char*)_alloca(outLen);
            WideCharToMultiByte(consoleCP, 0, wideBuf, -1, outBuf, outLen, nullptr, nullptr);
            fputs(outBuf, stdout);
            return;
        }
    }
    fputs(utf8Str, stdout);
}

void LogWrite(LogLevel level, const char* module, const char* fmt, ...)
{
    if (level < g_logLevel) {
        return;
    }

    const char* levelStr = "";
    switch (level) {
        case LogLevel::LOG_LEVEL_DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::LOG_LEVEL_INFO:  levelStr = "INFO";  break;
        case LogLevel::LOG_LEVEL_WARN:  levelStr = "WARN";  break;
        case LogLevel::LOG_LEVEL_ERROR: levelStr = "ERROR"; break;
    }

    time_t now = time(nullptr);
    struct tm* tm_info = localtime(&now);
    char timeBuf[32];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", tm_info);

    char header[128];
    snprintf(header, sizeof(header), "[%s][%s][%s] ", timeBuf, levelStr, module);
    PrintWithConsoleEncoding(header);

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);

    if (len > 0) {
        char* msgBuf = (char*)_alloca(len + 1);
        va_start(args, fmt);
        vsnprintf(msgBuf, len + 1, fmt, args);
        va_end(args);
        PrintWithConsoleEncoding(msgBuf);
    }

    PrintWithConsoleEncoding("\n");
    fflush(stdout);
}
