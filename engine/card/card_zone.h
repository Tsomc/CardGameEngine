#ifndef CARD_ZONE_H
#define CARD_ZONE_H

#include "card.h"
#include <vector>


namespace engine {

class CardZone {
public:
    virtual ~CardZone() = default;

    virtual bool ZoneAdd(Card* card);
    virtual bool ZoneRemove(CardInstanceId cardId);
    virtual Card* ZoneGet(CardInstanceId cardId) const;
    virtual Card* ZoneGetAt(int index) const;
    virtual int ZoneCount() const;
    virtual bool ZoneContains(CardInstanceId cardId) const;
    virtual void ZoneClear();
    virtual std::vector<Card*> ZoneGetAll() const;

protected:
    std::vector<Card*> cards;
};


} /* namespace engine */
#endif
