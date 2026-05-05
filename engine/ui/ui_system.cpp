#include "ui_system.h"
#include "core_log.h"
#include "event_bus.h"
#include <algorithm>

UISystem::UISystem()
    : nextElementId(1)
    , focusedElementId(0)
{
}

UISystem::~UISystem()
{
}

bool UISystem::ModuleInit()
{
    LOG_INFO("UI", "UI 系统初始化");
    return true;
}

void UISystem::ModuleStart()
{
}

void UISystem::ModuleUpdate(float deltaTime)
{
    for (auto& elem : rootElements) {
        if (elem->visible) {
            elem->UIUpdate(deltaTime);
        }
    }
}

void UISystem::ModuleRender()
{
    std::vector<UIElement*> sortedElements;
    for (auto& elem : rootElements) {
        if (elem->visible) {
            sortedElements.push_back(elem.get());
        }
    }

    std::sort(sortedElements.begin(), sortedElements.end(),
        [](UIElement* a, UIElement* b) {
            return a->layer < b->layer;
        });

    for (UIElement* elem : sortedElements) {
        elem->UIRender();
    }
}

void UISystem::ModuleShutdown()
{
    rootElements.clear();
    elementMap.clear();
    LOG_INFO("UI", "UI 系统关闭");
}

const char* UISystem::ModuleGetName() const
{
    return "UISystem";
}

uint32_t UISystem::UICreateFromLayout(const std::string& layoutPath)
{
    (void)layoutPath;
    LOG_WARN("UI", "UICreateFromLayout 尚未实现: %s", layoutPath.c_str());
    return 0;
}

void UISystem::UIDestroyElement(uint32_t id)
{
    for (auto it = rootElements.begin(); it != rootElements.end(); ++it) {
        if ((*it)->id == id) {
            elementMap.erase(id);
            rootElements.erase(it);
            return;
        }
    }
}

void UISystem::UIDestroyAll()
{
    rootElements.clear();
    elementMap.clear();
}

void UISystem::UIHandleEvent(const Event& event)
{
    (void)event;
}

void UISystem::UIRegisterWidgetType(
    const std::string& typeName,
    std::function<uint32_t()> factory)
{
    (void)typeName;
    (void)factory;
}

void UISystem::UISetFocus(uint32_t elementId)
{
    focusedElementId = elementId;
}

uint32_t UISystem::UIGetFocus() const
{
    return focusedElementId;
}

void UISystem::UIClearFocus()
{
    focusedElementId = 0;
}
