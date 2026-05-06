#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "module_manager.h"
#include "resource_handle.h"
#include "resource_entry.h"
#include <string>
#include <unordered_map>


namespace engine {

class ResourceManager : public IModule {
public:
    ResourceManager();
    ~ResourceManager() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    void ResourceSetRootPath(const std::string& rootPath);

    ResourceId ResourceLoadTexture(
        const std::string& path, const std::string& cacheId = "");
    ResourceId ResourceLoadSFX(
        const std::string& path, const std::string& cacheId = "");
    ResourceId ResourceLoadMusic(
        const std::string& path, const std::string& cacheId = "");
    ResourceId ResourceLoadFont(
        const std::string& path, int fontSize, const std::string& cacheId = "");
    ResourceId ResourceLoadJson(const std::string& path);

    bool ResourceIsLoaded(ResourceId id);
    void ResourceUnload(ResourceId id);
    void ResourceUnloadAll();
    ResourceId ResourceFind(const std::string& cacheId) const;

    template<typename T>
    T* ResourceGet(ResourceId id)
    {
        auto it = idMap.find(id);
        if (it == idMap.end()) {
            return nullptr;
        }

        auto cacheIt = cache.find(it->second);
        if (cacheIt == cache.end()) {
            return nullptr;
        }

        return static_cast<T*>(cacheIt->second.data);
    }

    int ResourceGetCacheSize() const;

private:
    std::string rootPath_;
    std::unordered_map<std::string, ResourceEntry> cache;
    std::unordered_map<ResourceId, std::string> idMap;
    ResourceId nextId;
};


} /* namespace engine */
#endif
