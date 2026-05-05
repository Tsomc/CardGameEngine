#include "resource_manager.h"
#include "resource_entry.h"
#include "core_log.h"
#include "engine.h"
#include "i_platform.h"
#include "platform_rwops.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <nlohmann/json.hpp>

ResourceManager::ResourceManager()
    : nextId(1)
{
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::ModuleInit()
{
    LOG_INFO("Resource", "资源管理器初始化");
    return true;
}

void ResourceManager::ModuleStart()
{
}

void ResourceManager::ModuleUpdate(float deltaTime)
{
    (void)deltaTime;
}

void ResourceManager::ModuleShutdown()
{
    ResourceUnloadAll();
    LOG_INFO("Resource", "资源管理器关闭");
}

const char* ResourceManager::ModuleGetName() const
{
    return "ResourceManager";
}

void ResourceManager::ResourceSetRootPath(const std::string& rootPath)
{
    rootPath_ = rootPath;
}

ResourceId ResourceManager::ResourceLoadTexture(
    const std::string& path, const std::string& cacheId)
{
    if (!cacheId.empty()) {
        ResourceId existing = ResourceFind(cacheId);
        if (existing != INVALID_RESOURCE_ID) {
            return existing;
        }
    }

    IPlatform& plat = Engine::EngineGet().EngineGetPlatform();
    SDL_Renderer* renderer = (SDL_Renderer*)plat.PlatformGetRenderer();
    if (renderer == nullptr) {
        LOG_ERROR("Resource", "无法加载纹理 %s: 渲染器为空", path.c_str());
        return INVALID_RESOURCE_ID;
    }

    std::string fullPath = std::string(plat.PlatformGetBasePath()) + path;
    SDL_Texture* texture = IMG_LoadTexture(renderer, fullPath.c_str());
    if (texture == nullptr) {
        LOG_ERROR("Resource", "加载纹理失败: %s (%s)", fullPath.c_str(), IMG_GetError());
        return INVALID_RESOURCE_ID;
    }

    ResourceEntry entry;
    entry.cacheId = cacheId.empty() ? path : cacheId;
    entry.filePath = path;
    entry.id = nextId++;
    entry.type = ResourceType::RESOURCE_TYPE_TEXTURE;
    entry.data = (void*)texture;
    entry.refCount = 1;

    cache[entry.cacheId] = entry;
    idMap[entry.id] = entry.cacheId;

    LOG_INFO("Resource", "加载纹理: %s (id=%u)", entry.cacheId.c_str(), entry.id);
    return entry.id;
}

ResourceId ResourceManager::ResourceLoadSFX(
    const std::string& path, const std::string& cacheId)
{
    if (!cacheId.empty()) {
        ResourceId existing = ResourceFind(cacheId);
        if (existing != INVALID_RESOURCE_ID) {
            return existing;
        }
    }

    IPlatform& plat = Engine::EngineGet().EngineGetPlatform();
    std::string fullPath = std::string(plat.PlatformGetBasePath()) + path;

    Mix_Chunk* chunk = Mix_LoadWAV(fullPath.c_str());
    if (chunk == nullptr) {
        LOG_ERROR("Resource", "加载音效失败: %s (%s)", fullPath.c_str(), Mix_GetError());
        return INVALID_RESOURCE_ID;
    }

    ResourceEntry entry;
    entry.cacheId = cacheId.empty() ? path : cacheId;
    entry.filePath = path;
    entry.id = nextId++;
    entry.type = ResourceType::RESOURCE_TYPE_SFX;
    entry.data = (void*)chunk;
    entry.refCount = 1;

    cache[entry.cacheId] = entry;
    idMap[entry.id] = entry.cacheId;

    LOG_INFO("Resource", "加载音效: %s (id=%u)", entry.cacheId.c_str(), entry.id);
    return entry.id;
}

ResourceId ResourceManager::ResourceLoadMusic(
    const std::string& path, const std::string& cacheId)
{
    if (!cacheId.empty()) {
        ResourceId existing = ResourceFind(cacheId);
        if (existing != INVALID_RESOURCE_ID) {
            return existing;
        }
    }

    IPlatform& plat = Engine::EngineGet().EngineGetPlatform();
    std::string fullPath = std::string(plat.PlatformGetBasePath()) + path;

    Mix_Music* music = Mix_LoadMUS(fullPath.c_str());
    if (music == nullptr) {
        LOG_ERROR("Resource", "加载音乐失败: %s (%s)", fullPath.c_str(), Mix_GetError());
        return INVALID_RESOURCE_ID;
    }

    ResourceEntry entry;
    entry.cacheId = cacheId.empty() ? path : cacheId;
    entry.filePath = path;
    entry.id = nextId++;
    entry.type = ResourceType::RESOURCE_TYPE_MUSIC;
    entry.data = (void*)music;
    entry.refCount = 1;

    cache[entry.cacheId] = entry;
    idMap[entry.id] = entry.cacheId;

    LOG_INFO("Resource", "加载音乐: %s (id=%u)", entry.cacheId.c_str(), entry.id);
    return entry.id;
}

ResourceId ResourceManager::ResourceLoadFont(
    const std::string& path, int fontSize, const std::string& cacheId)
{
    std::string key = cacheId.empty() ? path : cacheId;
    key += "@" + std::to_string(fontSize);

    ResourceId existing = ResourceFind(key);
    if (existing != INVALID_RESOURCE_ID) {
        return existing;
    }

    IPlatform& plat = Engine::EngineGet().EngineGetPlatform();
    std::string fullPath = std::string(plat.PlatformGetBasePath()) + path;

    TTF_Font* font = TTF_OpenFont(fullPath.c_str(), fontSize);
    if (font == nullptr) {
        LOG_ERROR("Resource", "加载字体失败: %s (%s)", fullPath.c_str(), TTF_GetError());
        return INVALID_RESOURCE_ID;
    }

    ResourceEntry entry;
    entry.cacheId = key;
    entry.filePath = path;
    entry.id = nextId++;
    entry.type = ResourceType::RESOURCE_TYPE_FONT;
    entry.data = (void*)font;
    entry.refCount = 1;

    cache[entry.cacheId] = entry;
    idMap[entry.id] = entry.cacheId;

    LOG_INFO("Resource", "加载字体: %s (id=%u, size=%d)", path.c_str(), entry.id, fontSize);
    return entry.id;
}

ResourceId ResourceManager::ResourceLoadJson(const std::string& path)
{
    ResourceId existing = ResourceFind(path);
    if (existing != INVALID_RESOURCE_ID) {
        return existing;
    }

    IPlatform& plat = Engine::EngineGet().EngineGetPlatform();
    void* rwops = plat.PlatformOpenFile(path.c_str());
    if (rwops == nullptr) {
        return INVALID_RESOURCE_ID;
    }

    RwopsGuard guard((SDL_RWops*)rwops);
    Sint64 size = SDL_RWsize(guard.Get());
    if (size <= 0) {
        LOG_ERROR("Resource", "JSON 文件为空: %s", path.c_str());
        return INVALID_RESOURCE_ID;
    }

    std::vector<char> buffer((size_t)size + 1, 0);
    if (SDL_RWread(guard.Get(), buffer.data(), 1, (size_t)size) != (size_t)size) {
        LOG_ERROR("Resource", "读取 JSON 文件失败: %s", path.c_str());
        return INVALID_RESOURCE_ID;
    }

    try {
        auto jsonData = new nlohmann::json();
        *jsonData = nlohmann::json::parse(buffer.data());

        ResourceEntry entry;
        entry.cacheId = path;
        entry.filePath = path;
        entry.id = nextId++;
        entry.type = ResourceType::RESOURCE_TYPE_JSON;
        entry.data = (void*)jsonData;
        entry.refCount = 1;

        cache[entry.cacheId] = entry;
        idMap[entry.id] = entry.cacheId;

        LOG_INFO("Resource", "加载 JSON: %s (id=%u)", path.c_str(), entry.id);
        return entry.id;
    } catch (const std::exception& e) {
        LOG_ERROR("Resource", "解析 JSON 失败: %s (%s)", path.c_str(), e.what());
        return INVALID_RESOURCE_ID;
    }
}

bool ResourceManager::ResourceIsLoaded(ResourceId id)
{
    return idMap.find(id) != idMap.end();
}

void ResourceManager::ResourceUnload(ResourceId id)
{
    auto it = idMap.find(id);
    if (it == idMap.end()) {
        return;
    }

    auto cacheIt = cache.find(it->second);
    if (cacheIt == cache.end()) {
        return;
    }

    ResourceEntry& entry = cacheIt->second;

    switch (entry.type) {
        case ResourceType::RESOURCE_TYPE_TEXTURE:
            if (entry.data != nullptr) {
                SDL_DestroyTexture((SDL_Texture*)entry.data);
            }
            break;
        case ResourceType::RESOURCE_TYPE_SFX:
            if (entry.data != nullptr) {
                Mix_FreeChunk((Mix_Chunk*)entry.data);
            }
            break;
        case ResourceType::RESOURCE_TYPE_MUSIC:
            if (entry.data != nullptr) {
                Mix_FreeMusic((Mix_Music*)entry.data);
            }
            break;
        case ResourceType::RESOURCE_TYPE_FONT:
            if (entry.data != nullptr) {
                TTF_CloseFont((TTF_Font*)entry.data);
            }
            break;
        case ResourceType::RESOURCE_TYPE_JSON:
            if (entry.data != nullptr) {
                delete (nlohmann::json*)entry.data;
            }
            break;
    }

    LOG_INFO("Resource", "卸载资源: %s (id=%u)", entry.cacheId.c_str(), id);
    cache.erase(cacheIt);
    idMap.erase(it);
}

void ResourceManager::ResourceUnloadAll()
{
    for (auto& pair : cache) {
        ResourceEntry& entry = pair.second;
        switch (entry.type) {
            case ResourceType::RESOURCE_TYPE_TEXTURE:
                if (entry.data != nullptr) {
                    SDL_DestroyTexture((SDL_Texture*)entry.data);
                }
                break;
            case ResourceType::RESOURCE_TYPE_SFX:
                if (entry.data != nullptr) {
                    Mix_FreeChunk((Mix_Chunk*)entry.data);
                }
                break;
            case ResourceType::RESOURCE_TYPE_MUSIC:
                if (entry.data != nullptr) {
                    Mix_FreeMusic((Mix_Music*)entry.data);
                }
                break;
            case ResourceType::RESOURCE_TYPE_FONT:
                if (entry.data != nullptr) {
                    TTF_CloseFont((TTF_Font*)entry.data);
                }
                break;
            case ResourceType::RESOURCE_TYPE_JSON:
                if (entry.data != nullptr) {
                    delete (nlohmann::json*)entry.data;
                }
                break;
        }
    }

    cache.clear();
    idMap.clear();
    nextId = 1;
}

ResourceId ResourceManager::ResourceFind(const std::string& cacheId) const
{
    auto it = cache.find(cacheId);
    if (it == cache.end()) {
        return INVALID_RESOURCE_ID;
    }
    return it->second.id;
}

int ResourceManager::ResourceGetCacheSize() const
{
    return (int)cache.size();
}
