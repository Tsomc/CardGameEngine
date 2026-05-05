#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H

#include "module_manager.h"
#include "core_log.h"
#include <string>
#include <unordered_set>
#include <memory>
#include <sol/sol.hpp>

class ScriptSystem : public IModule {
public:
    ScriptSystem();
    ~ScriptSystem() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    bool ScriptLoadFile(const std::string& path);
    bool ScriptExecuteString(const std::string& code);

    template<typename Ret, typename... Args>
    Ret ScriptCall(const std::string& funcName, Args... args)
    {
        if (luaState == nullptr) {
            return Ret{};
        }

        try {
            sol::protected_function func =
                luaState->get<sol::protected_function>(funcName);
            if (!func.valid()) {
                lastError = "Function not found: " + funcName;
                LOG_ERROR("Script", "函数不存在 %s", funcName.c_str());
                return Ret{};
            }

            sol::protected_function_result callResult = func.call(args...);
            if (!callResult.valid()) {
                sol::error err = callResult;
                lastError = err.what();
                LOG_ERROR("Script", "函数执行失败 %s: %s",
                          funcName.c_str(), lastError.c_str());
                return Ret{};
            }

            return callResult.get<Ret>();
        } catch (const sol::error& e) {
            lastError = e.what();
            LOG_ERROR("Script", "脚本异常 %s: %s",
                      funcName.c_str(), lastError.c_str());
            return Ret{};
        }
    }

    template<typename... Args>
    void ScriptCallVoid(const std::string& funcName, Args... args)
    {
        if (luaState == nullptr) {
            return;
        }

        try {
            sol::protected_function func =
                luaState->get<sol::protected_function>(funcName);
            if (!func.valid()) {
                return;
            }

            sol::protected_function_result result = func.call(args...);
            if (!result.valid()) {
                sol::error err = result;
                lastError = err.what();
                LOG_ERROR("Script", "函数执行失败 %s: %s",
                          funcName.c_str(), lastError.c_str());
            }
        } catch (const sol::error& e) {
            lastError = e.what();
            LOG_ERROR("Script", "脚本异常 %s: %s",
                      funcName.c_str(), lastError.c_str());
        }
    }

    template<typename T>
    T ScriptGetGlobal(const std::string& name)
    {
        if (luaState == nullptr) {
            return T();
        }
        return luaState->get<T>(name);
    }

    template<typename T>
    void ScriptSetGlobal(const std::string& name, const T& value)
    {
        if (luaState != nullptr) {
            (*luaState)[name] = value;
        }
    }

    template<typename T>
    void ScriptBindClass(const std::string& luaName)
    {
        if (luaState != nullptr) {
            luaState->new_usertype<T>(luaName);
        }
    }

    template<typename Func>
    void ScriptBindFunction(const std::string& luaName, Func&& func)
    {
        if (luaState != nullptr) {
            (*luaState)[luaName] = std::forward<Func>(func);
        }
    }

    bool ScriptHasFunction(const std::string& funcName);
    std::string ScriptGetLastError() const;
    void ScriptReload();
    void* ScriptGetLuaState();

    void ScriptSetMaxInstructions(uint32_t maxInstructions);
    uint32_t ScriptGetMaxInstructions() const;

private:
    std::unique_ptr<sol::state> luaState;
    std::unordered_set<std::string> loadedFiles;
    std::string lastError;
    uint32_t maxInstructions;
};

#endif
