#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "module_manager.h"
#include "ui_element.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

class UISystem : public IModule {
public:
    UISystem();
    ~UISystem() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleRender() override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    uint32_t UICreateFromLayout(const std::string& layoutPath);

    template<typename T, typename... Args>
    uint32_t UICreateElement(Args&&... args)
    {
        uint32_t id = nextElementId++;
        auto elem = std::make_unique<T>(std::forward<Args>(args)...);
        elem->id = id;
        elementMap[id] = elem.get();
        rootElements.push_back(std::move(elem));
        return id;
    }

    template<typename T>
    T* UIGetElement(uint32_t id)
    {
        auto it = elementMap.find(id);
        if (it == elementMap.end()) {
            return nullptr;
        }
        return dynamic_cast<T*>(it->second);
    }

    void UIDestroyElement(uint32_t id);
    void UIDestroyAll();
    void UIUpdate(float deltaTime);
    void UIRender();
    void UIHandleEvent(const Event& event);

    void UIRegisterWidgetType(
        const std::string& typeName,
        std::function<uint32_t()> factory);

    void UISetFocus(uint32_t elementId);
    uint32_t UIGetFocus() const;
    void UIClearFocus();

private:
    std::vector<std::unique_ptr<UIElement>> rootElements;
    std::unordered_map<uint32_t, UIElement*> elementMap;
    uint32_t nextElementId;
    uint32_t focusedElementId;
};

#endif
