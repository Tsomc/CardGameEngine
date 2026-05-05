#include "script_system.h"
#include "core_log.h"

#include <sol/sol.hpp>

ScriptSystem::ScriptSystem()
    : maxInstructions(1000000)
{
}

ScriptSystem::~ScriptSystem()
{
}

bool ScriptSystem::ModuleInit()
{
    LOG_INFO("Script", "脚本系统初始化");

    try {
        luaState = std::make_unique<sol::state>();
        luaState->open_libraries(
            sol::lib::base,
            sol::lib::string,
            sol::lib::table,
            sol::lib::math
        );

        ScriptBindFunction("Log_Debug",
            [](const std::string& msg) {
                LOG_DEBUG("Lua", "%s", msg.c_str());
            });

        ScriptBindFunction("Log_Info",
            [](const std::string& msg) {
                LOG_INFO("Lua", "%s", msg.c_str());
            });

        ScriptBindFunction("Log_Warn",
            [](const std::string& msg) {
                LOG_WARN("Lua", "%s", msg.c_str());
            });

        ScriptBindFunction("Log_Error",
            [](const std::string& msg) {
                LOG_ERROR("Lua", "%s", msg.c_str());
            });

        LOG_INFO("Script", "Lua 虚拟机初始化完成 (Lua %s)", LUA_VERSION);

    } catch (const std::exception& e) {
        LOG_ERROR("Script", "Lua 初始化失败: %s", e.what());
        return false;
    }

    return true;
}

void ScriptSystem::ModuleStart()
{
}

void ScriptSystem::ModuleUpdate(float deltaTime)
{
    (void)deltaTime;
}

void ScriptSystem::ModuleShutdown()
{
    luaState.reset();
    loadedFiles.clear();
    LOG_INFO("Script", "脚本系统关闭");
}

const char* ScriptSystem::ModuleGetName() const
{
    return "ScriptSystem";
}

bool ScriptSystem::ScriptLoadFile(const std::string& path)
{
    if (luaState == nullptr) {
        LOG_ERROR("Script", "Lua 状态未初始化");
        return false;
    }

    try {
        auto result = luaState->script_file(path);
        if (!result.valid()) {
            sol::error err = result;
            lastError = err.what();
            LOG_ERROR("Script", "加载脚本失败: %s (%s)",
                      path.c_str(), lastError.c_str());
            return false;
        }

        loadedFiles.insert(path);
        LOG_INFO("Script", "脚本已加载: %s", path.c_str());
        return true;

    } catch (const std::exception& e) {
        lastError = e.what();
        LOG_ERROR("Script", "脚本加载异常: %s (%s)", path.c_str(), lastError.c_str());
        return false;
    }
}

bool ScriptSystem::ScriptExecuteString(const std::string& code)
{
    if (luaState == nullptr) {
        return false;
    }

    try {
        auto result = luaState->script(code);
        if (!result.valid()) {
            sol::error err = result;
            lastError = err.what();
            LOG_ERROR("Script", "执行脚本代码失败: %s", lastError.c_str());
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        LOG_ERROR("Script", "执行脚本异常: %s", lastError.c_str());
        return false;
    }
}

bool ScriptSystem::ScriptHasFunction(const std::string& funcName)
{
    if (luaState == nullptr) {
        return false;
    }

    sol::object obj = luaState->get<sol::object>(funcName);
    return obj.is<sol::function>();
}

std::string ScriptSystem::ScriptGetLastError() const
{
    return lastError;
}

void ScriptSystem::ScriptReload()
{
    if (luaState == nullptr) {
        return;
    }

    LOG_INFO("Script", "热重载脚本");

    auto savedFiles = loadedFiles;

    luaState.reset();
    luaState = std::make_unique<sol::state>();
    luaState->open_libraries(
        sol::lib::base,
        sol::lib::string,
        sol::lib::table,
        sol::lib::math
    );

    loadedFiles.clear();
    lastError.clear();

    for (const auto& path : savedFiles) {
        if (!ScriptLoadFile(path)) {
            LOG_ERROR("Script", "热重载加载失败: %s", path.c_str());
        }
    }
}

void* ScriptSystem::ScriptGetLuaState()
{
    return (void*)luaState.get();
}

void ScriptSystem::ScriptSetMaxInstructions(uint32_t maxInstr)
{
    maxInstructions = maxInstr;
}

uint32_t ScriptSystem::ScriptGetMaxInstructions() const
{
    return maxInstructions;
}
