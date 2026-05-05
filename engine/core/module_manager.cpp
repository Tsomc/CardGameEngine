#include "module_manager.h"
#include "core_log.h"
#include <queue>

ModuleManager::ModuleManager()
    : initialized(false)
{
}

ModuleManager::~ModuleManager()
{
    if (initialized) {
        ModuleShutdownAll();
    }
}

int ModuleManager::DoRegister(
    std::unique_ptr<IModule> instance,
    const std::vector<int>& deps)
{
    int index = (int)modules.size();
    ModuleEntry entry;
    entry.instance = std::move(instance);
    entry.deps = deps;
    entry.initialized = false;
    modules.push_back(std::move(entry));
    return index;
}

IModule* ModuleManager::DoGet(int index)
{
    if (index < 0 || index >= (int)modules.size()) {
        return nullptr;
    }
    return modules[index].instance.get();
}

bool ModuleManager::ModuleInitAll()
{
    std::vector<int> order = TopologicalSort();
    if (order.empty() && modules.size() > 0) {
        LOG_ERROR("Core", "拓扑排序失败，存在循环依赖");
        return false;
    }

    std::vector<int> initializedIndices;

    for (int idx : order) {
        if (!modules[idx].instance->ModuleInit()) {
            LOG_ERROR("Core", "模块 %d 初始化失败，开始回滚", idx);
            for (auto it = initializedIndices.rbegin();
                 it != initializedIndices.rend(); ++it) {
                modules[*it].instance->ModuleShutdown();
                modules[*it].initialized = false;
            }
            return false;
        }
        modules[idx].initialized = true;
        initializedIndices.push_back(idx);
    }

    initialized = true;
    initOrder = order;
    return true;
}

void ModuleManager::ModuleStartAll()
{
    for (int idx : initOrder) {
        modules[idx].instance->ModuleStart();
    }
}

void ModuleManager::ModuleUpdateAll(float deltaTime)
{
    for (int idx : initOrder) {
        modules[idx].instance->ModuleUpdate(deltaTime);
    }
}

void ModuleManager::ModuleRenderAll()
{
    for (int idx : initOrder) {
        modules[idx].instance->ModuleRender();
    }
}

void ModuleManager::ModuleShutdownAll()
{
    for (auto it = initOrder.rbegin(); it != initOrder.rend(); ++it) {
        int idx = *it;
        if (modules[idx].initialized) {
            modules[idx].instance->ModuleShutdown();
            modules[idx].initialized = false;
        }
    }
    initialized = false;
    initOrder.clear();
}

void ModuleManager::ModuleShutdown(int index)
{
    if (index < 0 || index >= (int)modules.size()) {
        return;
    }
    if (modules[index].initialized) {
        modules[index].instance->ModuleShutdown();
        modules[index].initialized = false;
    }
}

std::vector<int> ModuleManager::TopologicalSort()
{
    int n = (int)modules.size();
    std::vector<int> inDegree(n, 0);
    std::vector<std::vector<int>> adj(n);

    for (int i = 0; i < n; ++i) {
        for (int dep : modules[i].deps) {
            adj[dep].push_back(i);
            inDegree[i]++;
        }
    }

    std::queue<int> q;
    for (int i = 0; i < n; ++i) {
        if (inDegree[i] == 0) {
            q.push(i);
        }
    }

    std::vector<int> result;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        result.push_back(u);

        for (int v : adj[u]) {
            inDegree[v]--;
            if (inDegree[v] == 0) {
                q.push(v);
            }
        }
    }

    if ((int)result.size() != n) {
        return {};
    }

    return result;
}
