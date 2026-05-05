#ifndef CARD_BATTLEFIELD_H
#define CARD_BATTLEFIELD_H

#include "card_zone.h"

class CardBattlefield : public CardZone {
public:
    int battlefieldMaxSlots;

    CardBattlefield() : battlefieldMaxSlots(7) {}

    bool BattlefieldIsFull() const;
    int BattlefieldAdd(Card* card, int position);
    Card* BattlefieldGetAt(int position) const;
    int BattlefieldGetPosition(CardInstanceId cardId) const;
};

#endif
