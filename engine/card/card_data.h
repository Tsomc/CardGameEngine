#ifndef CARD_DATA_H
#define CARD_DATA_H

#include <string>
#include <nlohmann/json.hpp>

using CardTemplateId = uint32_t;

struct CardData {
    CardTemplateId templateId;
    std::string name;
    int cost;
    std::string textureCacheId;
    std::string textureBackCacheId;
    std::string description;
    std::string scriptPath;
    nlohmann::json customData;

    int CardDataGetInt(const std::string& key, int defaultValue = 0) const
    {
        if (customData.contains(key) && customData[key].is_number()) {
            return customData[key].get<int>();
        }
        return defaultValue;
    }

    std::string CardDataGetString(
        const std::string& key,
        const std::string& defaultValue = "") const
    {
        if (customData.contains(key) && customData[key].is_string()) {
            return customData[key].get<std::string>();
        }
        return defaultValue;
    }
};

#endif
