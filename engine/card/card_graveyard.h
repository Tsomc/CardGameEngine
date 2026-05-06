#ifndef CARD_GRAVEYARD_H
#define CARD_GRAVEYARD_H

#include "card_zone.h"


namespace engine {

class CardGraveyard : public CardZone {
public:
    Card* GraveyardGetLast() const
    {
        if (cards.empty()) {
            return nullptr;
        }
        return cards.back();
    }
};


} /* namespace engine */
#endif
