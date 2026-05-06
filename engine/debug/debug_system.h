#ifndef DEBUG_SYSTEM_H
#define DEBUG_SYSTEM_H

#include "module_manager.h"
#include <string>
#include <vector>
#include <functional>
#include <deque>


namespace engine {

class DebugSystem : public IModule {
public:
    DebugSystem();
    ~DebugSystem() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleRender() override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    void DebugSetVisible(bool visible);
    bool DebugIsVisible() const;

    void DebugRegisterPanel(
        const std::string& name, std::function<void()> callback);
    void DebugUnregisterPanel(const std::string& name);

    void DebugLog(const std::string& message);

private:
    struct DebugPanel {
        std::string name;
        std::function<void()> callback;
    };

    struct EventRecord {
        float timestamp;
        std::string eventType;
        std::string summary;
    };

    struct PerfStats {
        float fps;
        float frameTimeMs;
        int drawCalls;
        int spriteCount;
    };

    void ShowPerformancePanel();
    void ShowEventLogPanel();
    void ShowConsolePanel();

    bool visible;
    std::vector<DebugPanel> registeredPanels;
    std::deque<EventRecord> eventHistory;
    std::vector<std::string> consoleBuffer;
    PerfStats perfData;
};


} /* namespace engine */
#endif
