#include "script_binder.h"
#include "script_system.h"
#include "core_log.h"
#include "engine.h"
#include <random>

void ScriptRegisterEngineBindings(ScriptSystem* script)
{
    if (script == nullptr) {
        return;
    }

    LOG_INFO("Script", "注册引擎内置 Lua 绑定");

    script->ScriptBindFunction("Random_Range",
        [](int min, int max) -> int {
            if (min > max) return min;
            std::mt19937_64& rng = Engine::EngineGet().EngineGetRNG();
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng);
        });

    script->ScriptBindFunction("Log_Debug",
        [](const std::string& msg) {
            LOG_DEBUG("Lua", "%s", msg.c_str());
        });

    script->ScriptBindFunction("Log_Info",
        [](const std::string& msg) {
            LOG_INFO("Lua", "%s", msg.c_str());
        });

    script->ScriptBindFunction("Log_Warn",
        [](const std::string& msg) {
            LOG_WARN("Lua", "%s", msg.c_str());
        });

    script->ScriptBindFunction("Log_Error",
        [](const std::string& msg) {
            LOG_ERROR("Lua", "%s", msg.c_str());
        });

    LOG_INFO("Script", "引擎内置 Lua 绑定注册完成");
}
