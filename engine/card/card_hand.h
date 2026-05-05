#ifndef CARD_HAND_H
#define CARD_HAND_H

#include "card_zone.h"
#include <functional>

class CardHand : public CardZone {
public:
    int handMaxSize;

    CardHand() : handMaxSize(10) {}

    bool HandIsFull() const;
    bool HandAdd(Card* card);
    void HandSort(std::function<bool(Card*, Card*)> comparator);
};

#endif
