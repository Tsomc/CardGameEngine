#include "card_system.h"
#include "card_deck.h"
#include "card_hand.h"
#include "card_battlefield.h"
#include "card_graveyard.h"
#include "core_log.h"
#include "engine.h"
#include "resource_manager.h"
#include "event_bus.h"

#include <nlohmann/json.hpp>

CardSystem::CardSystem()
    : nextInstanceId(1)
    , nextTemplateId(1)
{
}

CardSystem::~CardSystem()
{
    ModuleShutdown();
}

bool CardSystem::ModuleInit()
{
    LOG_INFO("Card", "卡牌系统初始化");

    /* Register base zones for 2 players */
    for (int playerId = 0; playerId < 2; ++playerId) {
        auto& zones = playerZones[playerId];
        zones[ZoneType::ZONE_DECK] = std::make_unique<CardDeck>();
        zones[ZoneType::ZONE_HAND] = std::make_unique<CardHand>();
        zones[ZoneType::ZONE_BATTLEFIELD] = std::make_unique<CardBattlefield>();
        zones[ZoneType::ZONE_GRAVEYARD] = std::make_unique<CardGraveyard>();
    }

    return true;
}

void CardSystem::ModuleStart()
{
}

void CardSystem::ModuleUpdate(float deltaTime)
{
    (void)deltaTime;
}

void CardSystem::ModuleShutdown()
{
    cardInstances.clear();
    cardTemplates.clear();
    highlightStates.clear();

    for (int i = 0; i < 2; ++i) {
        playerZones[i].clear();
    }

    for (auto& pair : customZones) {
        for (auto& zonePtr : pair.second) {
            zonePtr.second.reset();
        }
    }
    customZones.clear();

    LOG_INFO("Card", "卡牌系统关闭");
}

const char* CardSystem::ModuleGetName() const
{
    return "CardSystem";
}

bool CardSystem::CardLoadTemplates(const std::string& path)
{
    ResourceManager* res = Engine::EngineGet().EngineGetModule<ResourceManager>();
    if (res == nullptr) {
        LOG_ERROR("Card", "资源管理器不可用");
        return false;
    }

    ResourceId jsonId = res->ResourceLoadJson(path);
    if (jsonId == INVALID_RESOURCE_ID) {
        return false;
    }

    nlohmann::json* j = res->ResourceGet<nlohmann::json>(jsonId);
    if (j == nullptr) {
        return false;
    }

    try {
        if (!j->contains("cards")) {
            LOG_ERROR("Card", "JSON 缺少 cards 数组: %s", path.c_str());
            return false;
        }

        auto& cardsArray = (*j)["cards"];
        for (auto& cardJson : cardsArray) {
            CardData data;
            data.templateId = cardJson.value("templateId", (CardTemplateId)nextTemplateId++);
            data.name = cardJson.value("name", "");
            data.cost = cardJson.value("cost", 0);
            data.textureCacheId = cardJson.value("texture", "");
            data.textureBackCacheId = cardJson.value("textureBack", "");
            data.description = cardJson.value("description", "");
            data.scriptPath = cardJson.value("script", "");
            if (cardJson.contains("customData")) {
                data.customData = cardJson["customData"];
            }

            cardTemplates[data.templateId] = data;
        }

        LOG_INFO("Card", "已加载 %zu 张卡牌模板", cardTemplates.size());
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR("Card", "解析卡牌数据失败: %s", e.what());
        return false;
    }
}

CardInstanceId CardSystem::CardCreateInstance(
    CardTemplateId templateId, uint8_t owner, ZoneType zone, int position)
{
    auto tmplIt = cardTemplates.find(templateId);
    if (tmplIt == cardTemplates.end()) {
        LOG_ERROR("Card", "卡牌模板不存在: %u", templateId);
        return INVALID_CARD_INSTANCE_ID;
    }

    CardInstanceId newId = nextInstanceId++;
    auto card = std::make_unique<Card>(newId, &tmplIt->second);
    Card* rawPtr = card.get();

    rawPtr->CardSetOwner(owner);
    rawPtr->CardMoveToZone(zone, position);

    CardZone* targetZone = GetOrCreateZone(owner, zone);
    if (targetZone == nullptr) {
        LOG_ERROR("Card", "区域不存在: owner=%d zone=%d", owner, (int)zone);
        return INVALID_CARD_INSTANCE_ID;
    }

    if (!targetZone->ZoneAdd(rawPtr)) {
        LOG_WARN("Card", "卡牌添加区域失败: %u", newId);
        return INVALID_CARD_INSTANCE_ID;
    }

    cardInstances[newId] = std::move(card);

    CardZoneChangedEvent zcEvent;
    zcEvent.cardId = newId;
    zcEvent.fromZone = ZoneType::ZONE_DECK;
    zcEvent.toZone = zone;
    zcEvent.playerId = owner;
    Engine::EngineGet().EngineGetEventBus().EventEmit(zcEvent);

    return newId;
}

bool CardSystem::CardDestroyInstance(CardInstanceId cardId)
{
    auto it = cardInstances.find(cardId);
    if (it == cardInstances.end()) {
        return false;
    }

    Card* card = it->second.get();
    uint8_t owner = card->CardGetOwner();
    ZoneType currentZone = card->CardGetZone();

    CardZone* zone = GetOrCreateZone(owner, currentZone);
    if (zone != nullptr) {
        zone->ZoneRemove(cardId);
    }

    highlightStates.erase(cardId);

    CardDestroyedEvent event;
    event.cardId = cardId;
    event.sourceCardId = INVALID_CARD_INSTANCE_ID;
    event.ownerId = owner;
    Engine::EngineGet().EngineGetEventBus().EventEmit(event);

    cardInstances.erase(it);
    return true;
}

Card* CardSystem::CardGetInstance(CardInstanceId cardId) const
{
    auto it = cardInstances.find(cardId);
    if (it == cardInstances.end()) {
        return nullptr;
    }
    return it->second.get();
}

const CardData* CardSystem::CardGetTemplate(CardTemplateId templateId) const
{
    auto it = cardTemplates.find(templateId);
    if (it == cardTemplates.end()) {
        return nullptr;
    }
    return &it->second;
}

std::vector<Card*> CardSystem::CardGetCardsInZone(
    uint8_t playerId, ZoneType zone) const
{
    CardZone* zonePtr = GetZone(playerId, zone);
    if (zonePtr == nullptr) {
        return {};
    }
    return zonePtr->ZoneGetAll();
}

int CardSystem::CardGetZoneCount(uint8_t playerId, ZoneType zone) const
{
    CardZone* zonePtr = GetZone(playerId, zone);
    if (zonePtr == nullptr) {
        return 0;
    }
    return zonePtr->ZoneCount();
}

CardZone* CardSystem::CardGetZone(uint8_t playerId, ZoneType zone)
{
    return GetOrCreateZone(playerId, zone);
}

bool CardSystem::CardRegisterZone(ZoneType zoneType, std::unique_ptr<CardZone> zone)
{
    if (zoneType < ZoneType::ZONE_CUSTOM_START) {
        LOG_ERROR("Card", "不允许覆盖引擎内置区域: %d", (int)zoneType);
        return false;
    }

    customZones[0][zoneType] = std::move(zone);
    return true;
}

int CardSystem::CardGetInstanceCount() const
{
    return (int)cardInstances.size();
}

int CardSystem::CardGetTemplateCount() const
{
    return (int)cardTemplates.size();
}

std::vector<Card*> CardSystem::CardGetAllInstances() const
{
    std::vector<Card*> result;
    for (auto& pair : cardInstances) {
        result.push_back(pair.second.get());
    }
    return result;
}

void CardSystem::CardDestroyInstances(const std::vector<CardInstanceId>& cardIds)
{
    for (CardInstanceId id : cardIds) {
        CardDestroyInstance(id);
    }
}

int CardSystem::CardMoveCardsToZone(
    const std::vector<CardInstanceId>& cardIds, ZoneType zone)
{
    int moved = 0;
    for (CardInstanceId id : cardIds) {
        auto it = cardInstances.find(id);
        if (it == cardInstances.end()) {
            continue;
        }

        Card* card = it->second.get();
        uint8_t owner = card->CardGetOwner();

        CardZone* targetZone = GetOrCreateZone(owner, zone);
        if (targetZone == nullptr) {
            continue;
        }

        CardZone* currentZone = GetOrCreateZone(owner, card->CardGetZone());
        if (currentZone != nullptr) {
            currentZone->ZoneRemove(id);
        }

        card->CardMoveToZone(zone, -1);

        if (targetZone->ZoneAdd(card)) {
            CardZoneChangedEvent zcEvent;
            zcEvent.cardId = id;
            zcEvent.fromZone = card->CardGetZone();
            zcEvent.toZone = zone;
            zcEvent.playerId = owner;
            Engine::EngineGet().EngineGetEventBus().EventEmit(zcEvent);
            moved++;
        }
    }
    return moved;
}

void CardSystem::CardSetHighlight(
    CardInstanceId cardId, const Color& color, float intensity)
{
    if (intensity <= 0.0f ||
        (color.a == 0 && color.r == 0 && color.g == 0 && color.b == 0)) {
        highlightStates.erase(cardId);
    } else {
        highlightStates[cardId] = {color, intensity};
    }
}

void CardSystem::CardClearAllInstances()
{
    for (auto& pair : cardInstances) {
        Card* card = pair.second.get();
        uint8_t owner = card->CardGetOwner();
        ZoneType zone = card->CardGetZone();
        CardZone* zonePtr = GetOrCreateZone(owner, zone);
        if (zonePtr != nullptr) {
            zonePtr->ZoneRemove(pair.first);
        }
    }
    cardInstances.clear();
    highlightStates.clear();
}

bool CardSystem::CardInitMatch(const std::string& deckJsonPath)
{
    if (!cardInstances.empty()) {
        CardClearAllInstances();
    }

    ResourceManager* res = Engine::EngineGet().EngineGetModule<ResourceManager>();
    if (res == nullptr) {
        return false;
    }

    ResourceId jsonId = res->ResourceLoadJson(deckJsonPath);
    if (jsonId == INVALID_RESOURCE_ID) {
        return false;
    }

    nlohmann::json* j = res->ResourceGet<nlohmann::json>(jsonId);
    if (j == nullptr) {
        return false;
    }

    try {
        auto& players = (*j)["players"];
        for (auto& player : players) {
            uint8_t playerId = player["id"].get<uint8_t>();
            auto& deck = player["deck"];
            for (auto& tmplId : deck) {
                CardCreateInstance(
                    tmplId.get<CardTemplateId>(),
                    playerId,
                    ZoneType::ZONE_DECK,
                    -1
                );
            }
        }

        LOG_INFO("Card", "对局初始化完成，共 %zu 张卡牌", cardInstances.size());
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR("Card", "初始化对局失败: %s", e.what());
        return false;
    }
}

bool CardSystem::CardReturnToHand(CardInstanceId cardId)
{
    auto it = cardInstances.find(cardId);
    if (it == cardInstances.end()) {
        return false;
    }

    Card* card = it->second.get();
    uint8_t owner = card->CardGetOwner();

    CardHand* hand = dynamic_cast<CardHand*>(GetOrCreateZone(owner, ZoneType::ZONE_HAND));
    if (hand == nullptr) {
        return false;
    }

    if (hand->HandIsFull()) {
        return false;
    }

    CardZone* currentZone = GetOrCreateZone(owner, card->CardGetZone());
    if (currentZone != nullptr) {
        currentZone->ZoneRemove(cardId);
    }

    card->CardMoveToZone(ZoneType::ZONE_HAND, -1);
    hand->ZoneAdd(card);

    CardZoneChangedEvent zcEvent;
    zcEvent.cardId = cardId;
    zcEvent.fromZone = card->CardGetZone();
    zcEvent.toZone = ZoneType::ZONE_HAND;
    zcEvent.playerId = owner;
    Engine::EngineGet().EngineGetEventBus().EventEmit(zcEvent);

    return true;
}

CardInstanceId CardSystem::CardCloneInstance(
    CardInstanceId sourceCardId, uint8_t targetOwner,
    ZoneType targetZone, int targetPosition)
{
    auto it = cardInstances.find(sourceCardId);
    if (it == cardInstances.end()) {
        return INVALID_CARD_INSTANCE_ID;
    }

    Card* source = it->second.get();
    if (source->CardGetData() == nullptr) {
        return INVALID_CARD_INSTANCE_ID;
    }

    CardInstanceId newId = CardCreateInstance(
        source->CardGetData()->templateId,
        targetOwner,
        targetZone,
        targetPosition
    );

    if (newId == INVALID_CARD_INSTANCE_ID) {
        return INVALID_CARD_INSTANCE_ID;
    }

    Card* clone = cardInstances[newId].get();
    clone->CardSetFaceDown(source->CardIsFaceDown());
    clone->CardClearAllFlags();
    clone->CardSetCustomData(source->CardGetCustomData());

    return newId;
}

std::vector<Card*> CardSystem::CardFindInstances(
    uint8_t playerId, ZoneType zone,
    std::function<bool(const Card*)> predicate)
{
    std::vector<Card*> result;
    CardZone* zonePtr = GetZone(playerId, zone);
    if (zonePtr == nullptr) {
        return result;
    }

    auto cards = zonePtr->ZoneGetAll();
    for (Card* card : cards) {
        if (predicate(card)) {
            result.push_back(card);
        }
    }
    return result;
}

std::vector<Card*> CardSystem::CardFindInstances(
    uint8_t playerId,
    std::function<bool(const Card*)> predicate)
{
    std::vector<Card*> result;

    for (int z = (int)ZoneType::ZONE_DECK; z <= (int)ZoneType::ZONE_GRAVEYARD; ++z) {
        auto zoneCards = CardFindInstances(
            playerId, (ZoneType)z, predicate);
        result.insert(result.end(), zoneCards.begin(), zoneCards.end());
    }

    for (auto& pair : customZones) {
        if (pair.first == playerId) {
            for (auto& zonePair : pair.second) {
                auto cards = zonePair.second->ZoneGetAll();
                for (Card* card : cards) {
                    if (predicate(card)) {
                        result.push_back(card);
                    }
                }
            }
        }
    }

    return result;
}

CardZone* CardSystem::GetZone(uint8_t playerId, ZoneType zone) const
{
    auto pIt = playerZones.find(playerId);
    if (pIt != playerZones.end()) {
        auto zIt = pIt->second.find(zone);
        if (zIt != pIt->second.end()) {
            return zIt->second.get();
        }
    }

    auto cIt = customZones.find(playerId);
    if (cIt != customZones.end()) {
        auto zIt = cIt->second.find(zone);
        if (zIt != cIt->second.end()) {
            return zIt->second.get();
        }
    }

    return nullptr;
}

CardZone* CardSystem::GetOrCreateZone(uint8_t playerId, ZoneType zone)
{
    CardZone* zonePtr = GetZone(playerId, zone);
    if (zonePtr != nullptr) {
        return zonePtr;
    }

    if ((int)zone >= (int)ZoneType::ZONE_CUSTOM_START) {
        auto newZone = std::make_unique<CardZone>();
        zonePtr = newZone.get();
        customZones[playerId][zone] = std::move(newZone);
        return zonePtr;
    }

    return nullptr;
}
