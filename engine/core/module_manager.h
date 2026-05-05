#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <memory>
#include <vector>

class IModule {
public:
    virtual ~IModule() = default;

    virtual bool ModuleInit() = 0;
    virtual void ModuleStart() = 0;
    virtual void ModuleUpdate(float deltaTime) = 0;
    virtual void ModuleRender() {}
    virtual void ModuleShutdown() = 0;

    virtual const char* ModuleGetName() const = 0;
};

class ModuleManager {
public:
    ModuleManager();
    ~ModuleManager();

    template<typename T, typename... Deps, typename... Args>
    int ModuleRegister(Args&&... args)
    {
        std::vector<int> deps = { ModuleRegisterDeps<Deps>()... };
        return DoRegister(
            std::make_unique<T>(std::forward<Args>(args)...),
            deps
        );
    }

    template<typename T>
    T* ModuleGet()
    {
        for (int i = 0; i < (int)modules.size(); ++i) {
            T* ptr = dynamic_cast<T*>(modules[i].instance.get());
            if (ptr != nullptr) {
                return ptr;
            }
        }
        return nullptr;
    }

    bool ModuleInitAll();
    void ModuleStartAll();
    void ModuleUpdateAll(float deltaTime);
    void ModuleRenderAll();
    void ModuleShutdownAll();

    void ModuleShutdown(int index);

private:
    struct ModuleEntry {
        std::unique_ptr<IModule> instance;
        std::vector<int> deps;
        bool initialized;
    };

    template<typename T>
    int ModuleRegisterDeps()
    {
        return ModuleGet<T>() != nullptr
            ? 0
            : ModuleRegister<T>();
    }

    int DoRegister(
        std::unique_ptr<IModule> instance,
        const std::vector<int>& deps);

    IModule* DoGet(int index);

    std::vector<int> TopologicalSort();

    std::vector<ModuleEntry> modules;
    std::vector<int> initOrder;
    bool initialized;
};

#endif
