#ifndef CARD_H
#define CARD_H

#include "card_data.h"
#include <string>
#include <unordered_map>
#include <cstdint>
#include <nlohmann/json.hpp>

using CardInstanceId = uint32_t;
constexpr CardInstanceId INVALID_CARD_INSTANCE_ID = 0;

enum class ZoneType : uint8_t {
    ZONE_DECK          = 0,
    ZONE_HAND          = 1,
    ZONE_BATTLEFIELD   = 2,
    ZONE_GRAVEYARD     = 3,
    ZONE_CUSTOM_START  = 16
};

class Card {
public:
    Card(CardInstanceId id, const CardData* data);

    CardInstanceId CardGetInstanceId() const;
    const CardData* CardGetData() const;
    void CardSetOwner(uint8_t owner);
    uint8_t CardGetOwner() const;
    void CardSetZone(ZoneType zone);
    ZoneType CardGetZone() const;
    void CardMoveToZone(ZoneType zone, int position = -1);

    /* Face down */
    void CardSetFaceDown(bool faceDown);
    bool CardIsFaceDown() const;

    /* Properties */
    void CardSetInt(const std::string& key, int value);
    int CardGetInt(const std::string& key, int defaultValue = 0) const;

    /* Flags */
    void CardSetFlags(uint64_t flags);
    void CardClearFlags(uint64_t flags);
    bool CardHasFlags(uint64_t flags) const;
    uint64_t CardGetAllFlags() const;
    void CardClearAllFlags();

    /* Counters */
    void CardAddCounter(const std::string& key, int amount);
    void CardSetCounter(const std::string& key, int value);
    int CardGetCounter(const std::string& key, int defaultValue = 0) const;
    void CardClearCounter(const std::string& key);
    void CardClearAllCounters();
    const std::unordered_map<std::string, int>& CardGetAllCounters() const;

    /* Custom data */
    void CardSetCustomData(const nlohmann::json& data);
    const nlohmann::json& CardGetCustomData() const;

private:
    CardInstanceId instanceId;
    const CardData* data;
    uint8_t owner;
    ZoneType zone;
    bool faceDown;

    std::unordered_map<std::string, int> intProperties;
    uint64_t flags;
    nlohmann::json customData;
    std::unordered_map<std::string, int> counters;
};

#endif
