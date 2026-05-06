#include "card_hand.h"
#include <algorithm>
using namespace engine;
bool CardHand::HandAdd(Card* card)
{
    if (card == nullptr) {
        return false;
    }

    if (handMaxSize > 0 && (int)cards.size() >= handMaxSize) {
        return false;
    }

    cards.push_back(card);
    return true;
}

bool CardHand::HandIsFull() const
{
    return handMaxSize > 0 && (int)cards.size() >= handMaxSize;
}

void CardHand::HandSort(std::function<bool(Card*, Card*)> comparator)
{
    std::sort(cards.begin(), cards.end(), comparator);
}
