#ifndef CARD_SYSTEM_H
#define CARD_SYSTEM_H

#include "module_manager.h"
#include "event_bus.h"
#include "card.h"
#include "card_zone.h"
#include "math_types.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

struct CardPlayedEvent : Event {
    CardInstanceId cardId;
    uint8_t playerId;
    int position;
    CardInstanceId targetCardId;
};

struct CardDrawnEvent : Event {
    CardInstanceId cardId;
    uint8_t playerId;
    int handSize;
};

struct CardDestroyedEvent : Event {
    CardInstanceId cardId;
    CardInstanceId sourceCardId;
    uint8_t ownerId;
};

struct CardDiscardedEvent : Event {
    CardInstanceId cardId;
    uint8_t playerId;
};

struct CardZoneChangedEvent : Event {
    CardInstanceId cardId;
    ZoneType fromZone;
    ZoneType toZone;
    uint8_t playerId;
};

struct HighlightState {
    Color color;
    float intensity;
};

class CardSystem : public IModule {
public:
    CardSystem();
    ~CardSystem() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    bool CardLoadTemplates(const std::string& path);

    CardInstanceId CardCreateInstance(
        CardTemplateId templateId,
        uint8_t owner,
        ZoneType zone,
        int position = -1);

    bool CardDestroyInstance(CardInstanceId cardId);
    Card* CardGetInstance(CardInstanceId cardId) const;
    const CardData* CardGetTemplate(CardTemplateId templateId) const;

    std::vector<Card*> CardGetCardsInZone(
        uint8_t playerId, ZoneType zone) const;
    int CardGetZoneCount(uint8_t playerId, ZoneType zone) const;
    CardZone* CardGetZone(uint8_t playerId, ZoneType zone);

    bool CardRegisterZone(ZoneType zoneType, std::unique_ptr<CardZone> zone);

    int CardGetInstanceCount() const;
    int CardGetTemplateCount() const;
    std::vector<Card*> CardGetAllInstances() const;

    void CardDestroyInstances(const std::vector<CardInstanceId>& cardIds);
    int CardMoveCardsToZone(
        const std::vector<CardInstanceId>& cardIds, ZoneType zone);

    void CardSetHighlight(
        CardInstanceId cardId, const Color& color, float intensity = 0.4f);

    void CardClearAllInstances();
    bool CardInitMatch(const std::string& deckJsonPath);
    bool CardReturnToHand(CardInstanceId cardId);

    CardInstanceId CardCloneInstance(
        CardInstanceId sourceCardId,
        uint8_t targetOwner,
        ZoneType targetZone,
        int targetPosition = -1);

    std::vector<Card*> CardFindInstances(
        uint8_t playerId, ZoneType zone,
        std::function<bool(const Card*)> predicate);

    std::vector<Card*> CardFindInstances(
        uint8_t playerId,
        std::function<bool(const Card*)> predicate);

private:
    CardZone* GetZone(uint8_t playerId, ZoneType zone) const;
    CardZone* GetOrCreateZone(uint8_t playerId, ZoneType zone);

    std::unordered_map<CardTemplateId, CardData> cardTemplates;
    std::unordered_map<CardInstanceId, std::unique_ptr<Card>> cardInstances;

    std::unordered_map<uint8_t,
        std::unordered_map<ZoneType, std::unique_ptr<CardZone>>> playerZones;

    std::unordered_map<uint8_t,
        std::unordered_map<ZoneType, std::unique_ptr<CardZone>>> customZones;

    std::unordered_map<CardInstanceId, HighlightState> highlightStates;

    CardInstanceId nextInstanceId;
    CardTemplateId nextTemplateId;
};

#endif
