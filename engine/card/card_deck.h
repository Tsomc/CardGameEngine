#ifndef CARD_DECK_H
#define CARD_DECK_H

#include "card_zone.h"

class CardDeck : public CardZone {
public:
    void DeckShuffle();
    Card* DeckDrawTop();
    Card* DeckPeekTop() const;
    void DeckInsertRandom(Card* card);
};

#endif
