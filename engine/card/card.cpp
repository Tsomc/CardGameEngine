#include "card.h"
using namespace engine;
Card::Card(CardInstanceId id, const CardData* data)
    : instanceId(id)
    , data(data)
    , owner(0)
    , zone(ZoneType::ZONE_DECK)
    , faceDown(false)
    , flags(0)
{
}

CardInstanceId Card::CardGetInstanceId() const
{
    return instanceId;
}

const CardData* Card::CardGetData() const
{
    return data;
}

void Card::CardSetOwner(uint8_t ownerId)
{
    owner = ownerId;
}

uint8_t Card::CardGetOwner() const
{
    return owner;
}

void Card::CardSetZone(ZoneType z)
{
    zone = z;
}

ZoneType Card::CardGetZone() const
{
    return zone;
}

void Card::CardMoveToZone(ZoneType z, int position)
{
    (void)position;
    zone = z;
}

void Card::CardSetFaceDown(bool fd)
{
    faceDown = fd;
}

bool Card::CardIsFaceDown() const
{
    return faceDown;
}

void Card::CardSetInt(const std::string& key, int value)
{
    intProperties[key] = value;
}

int Card::CardGetInt(const std::string& key, int defaultValue) const
{
    auto it = intProperties.find(key);
    if (it != intProperties.end()) {
        return it->second;
    }
    return defaultValue;
}

void Card::CardSetFlags(uint64_t f)
{
    flags |= f;
}

void Card::CardClearFlags(uint64_t f)
{
    flags &= ~f;
}

bool Card::CardHasFlags(uint64_t f) const
{
    return (flags & f) != 0;
}

uint64_t Card::CardGetAllFlags() const
{
    return flags;
}

void Card::CardClearAllFlags()
{
    flags = 0;
}

void Card::CardAddCounter(const std::string& key, int amount)
{
    counters[key] += amount;
}

void Card::CardSetCounter(const std::string& key, int value)
{
    counters[key] = value;
}

int Card::CardGetCounter(const std::string& key, int defaultValue) const
{
    auto it = counters.find(key);
    if (it != counters.end()) {
        return it->second;
    }
    return defaultValue;
}

void Card::CardClearCounter(const std::string& key)
{
    counters.erase(key);
}

void Card::CardClearAllCounters()
{
    counters.clear();
}

const std::unordered_map<std::string, int>& Card::CardGetAllCounters() const
{
    return counters;
}

void Card::CardSetCustomData(const nlohmann::json& d)
{
    customData = d;
}

const nlohmann::json& Card::CardGetCustomData() const
{
    return customData;
}
