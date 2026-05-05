#include "debug_system.h"
#include "core_log.h"
#include "engine.h"
#include "render_system.h"
#include "card_system.h"
#include "resource_manager.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <algorithm>

DebugSystem::DebugSystem()
    : visible(false)
{
}

DebugSystem::~DebugSystem()
{
}

bool DebugSystem::ModuleInit()
{
    LOG_INFO("Debug", "调试系统初始化");

    void* window = Engine::EngineGet().EngineGetPlatform().PlatformGetWindow();
    void* renderer = Engine::EngineGet().EngineGetPlatform().PlatformGetRenderer();

    if (window == nullptr || renderer == nullptr) {
        LOG_WARN("Debug", "窗口或渲染器不可用，ImGui 不可用");
        return true;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(
        (SDL_Window*)window, (SDL_Renderer*)renderer);
    ImGui_ImplSDLRenderer2_Init((SDL_Renderer*)renderer);

    ImGui::StyleColorsDark();

    LOG_INFO("Debug", "ImGui 初始化完成 (%s)", IMGUI_VERSION);
    return true;
}

void DebugSystem::ModuleStart()
{
}

void DebugSystem::ModuleUpdate(float deltaTime)
{
    (void)deltaTime;
}

void DebugSystem::ModuleRender()
{
    if (!visible) {
        return;
    }

    SDL_Window* window = (SDL_Window*)Engine::EngineGet().EngineGetPlatform().PlatformGetWindow();
    SDL_Renderer* renderer = (SDL_Renderer*)Engine::EngineGet().EngineGetPlatform().PlatformGetRenderer();
    if (window == nullptr || renderer == nullptr) {
        return;
    }

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ShowPerformancePanel();
    ShowEventLogPanel();

    for (auto& panel : registeredPanels) {
        if (panel.callback) {
            panel.callback();
        }
    }

    ImGui::Render();
    SDL_Renderer* sdlRenderer = (SDL_Renderer*)Engine::EngineGet().EngineGetPlatform().PlatformGetRenderer();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
}

void DebugSystem::ModuleShutdown()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    registeredPanels.clear();
    eventHistory.clear();
    consoleBuffer.clear();

    LOG_INFO("Debug", "调试系统关闭");
}

const char* DebugSystem::ModuleGetName() const
{
    return "DebugSystem";
}

void DebugSystem::DebugSetVisible(bool v)
{
    visible = v;
    LOG_INFO("Debug", "调试面板 %s", visible ? "显示" : "隐藏");
}

bool DebugSystem::DebugIsVisible() const
{
    return visible;
}

void DebugSystem::DebugRegisterPanel(
    const std::string& name, std::function<void()> callback)
{
    registeredPanels.push_back({name, callback});
}

void DebugSystem::DebugUnregisterPanel(const std::string& name)
{
    registeredPanels.erase(
        std::remove_if(registeredPanels.begin(), registeredPanels.end(),
            [&](const DebugPanel& p) { return p.name == name; }),
        registeredPanels.end()
    );
}

void DebugSystem::DebugLog(const std::string& message)
{
    (void)message;
}

void DebugSystem::ShowPerformancePanel()
{
    if (ImGui::Begin("性能面板", &visible)) {
        RenderSystem* render = Engine::EngineGet().EngineGetModule<RenderSystem>();
        CardSystem* card = Engine::EngineGet().EngineGetModule<CardSystem>();
        ResourceManager* res = Engine::EngineGet().EngineGetModule<ResourceManager>();

        if (render != nullptr) {
            ImGui::Text("Draw Calls: %d", render->RenderGetDrawCallCount());
            ImGui::Text("精灵数: %d", render->RenderGetSpriteCount());
        }

        if (card != nullptr) {
            ImGui::Text("卡牌实例: %d", card->CardGetInstanceCount());
            ImGui::Text("卡牌模板: %d", card->CardGetTemplateCount());
        }

        if (res != nullptr) {
            ImGui::Text("已加载资源: %d", res->ResourceGetCacheSize());
        }
    }
    ImGui::End();
}

void DebugSystem::ShowEventLogPanel()
{
    if (ImGui::Begin("事件日志", &visible)) {
        ImGui::Text("最近事件 (最多 200 条)");
        ImGui::Separator();

        if (ImGui::Button("清空")) {
            eventHistory.clear();
        }

        ImGui::BeginChild("ScrollingRegion");
        for (auto it = eventHistory.rbegin(); it != eventHistory.rend(); ++it) {
            char buf[64];
            snprintf(buf, sizeof(buf), "[%.3f] %s", it->timestamp, it->eventType.c_str());
            ImGui::Text("%s", buf);
            if (!it->summary.empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("-- %s", it->summary.c_str());
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void DebugSystem::ShowConsolePanel()
{
    if (ImGui::Begin("控制台", &visible)) {
        for (const auto& line : consoleBuffer) {
            ImGui::Text("%s", line.c_str());
        }
    }
    ImGui::End();
}
