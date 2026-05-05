#include "card_battlefield.h"

bool CardBattlefield::BattlefieldIsFull() const
{
    return battlefieldMaxSlots > 0 && (int)cards.size() >= battlefieldMaxSlots;
}

int CardBattlefield::BattlefieldAdd(Card* card, int position)
{
    if (card == nullptr) {
        return -1;
    }

    if (battlefieldMaxSlots > 0 && (int)cards.size() >= battlefieldMaxSlots) {
        return -1;
    }

    if (position < 0 || position > (int)cards.size()) {
        cards.push_back(card);
        return (int)cards.size() - 1;
    }

    cards.insert(cards.begin() + position, card);
    return position;
}

Card* CardBattlefield::BattlefieldGetAt(int position) const
{
    if (position < 0 || position >= (int)cards.size()) {
        return nullptr;
    }
    return cards[position];
}

int CardBattlefield::BattlefieldGetPosition(CardInstanceId cardId) const
{
    for (int i = 0; i < (int)cards.size(); ++i) {
        if (cards[i] != nullptr && cards[i]->CardGetInstanceId() == cardId) {
            return i;
        }
    }
    return -1;
}
