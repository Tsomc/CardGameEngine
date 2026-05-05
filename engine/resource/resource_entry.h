#ifndef RESOURCE_ENTRY_H
#define RESOURCE_ENTRY_H

#include "resource_handle.h"
#include <string>

enum class ResourceType : uint8_t {
    RESOURCE_TYPE_TEXTURE = 0,
    RESOURCE_TYPE_SFX     = 1,
    RESOURCE_TYPE_MUSIC   = 2,
    RESOURCE_TYPE_FONT    = 3,
    RESOURCE_TYPE_JSON    = 4
};

struct ResourceEntry {
    std::string cacheId;
    std::string filePath;
    ResourceId id;
    ResourceType type;
    void* data;
    int refCount;

    ResourceEntry()
        : id(INVALID_RESOURCE_ID)
        , type(ResourceType::RESOURCE_TYPE_TEXTURE)
        , data(nullptr)
        , refCount(0)
    {
    }
};

#endif
