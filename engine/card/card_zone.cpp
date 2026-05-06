#include "card_zone.h"
#include <algorithm>
using namespace engine;
bool CardZone::ZoneAdd(Card* card)
{
    if (card == nullptr) {
        return false;
    }
    cards.push_back(card);
    return true;
}

bool CardZone::ZoneRemove(CardInstanceId cardId)
{
    auto it = std::find_if(cards.begin(), cards.end(),
        [cardId](Card* c) { return c != nullptr && c->CardGetInstanceId() == cardId; });

    if (it == cards.end()) {
        return false;
    }

    cards.erase(it);
    return true;
}

Card* CardZone::ZoneGet(CardInstanceId cardId) const
{
    for (Card* c : cards) {
        if (c != nullptr && c->CardGetInstanceId() == cardId) {
            return c;
        }
    }
    return nullptr;
}

Card* CardZone::ZoneGetAt(int index) const
{
    if (index < 0 || index >= (int)cards.size()) {
        return nullptr;
    }
    return cards[index];
}

int CardZone::ZoneCount() const
{
    return (int)cards.size();
}

bool CardZone::ZoneContains(CardInstanceId cardId) const
{
    for (Card* c : cards) {
        if (c != nullptr && c->CardGetInstanceId() == cardId) {
            return true;
        }
    }
    return false;
}

void CardZone::ZoneClear()
{
    cards.clear();
}

std::vector<Card*> CardZone::ZoneGetAll() const
{
    return cards;
}
