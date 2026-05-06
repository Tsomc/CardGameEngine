#include "card_deck.h"
#include "engine.h"
#include <algorithm>
#include <random>
using namespace engine;
void CardDeck::DeckShuffle()
{
    std::mt19937_64& rng = Engine::EngineGet().EngineGetRNG();
    std::shuffle(cards.begin(), cards.end(), rng);
}

Card* CardDeck::DeckDrawTop()
{
    if (cards.empty()) {
        return nullptr;
    }
    Card* top = cards.back();
    cards.pop_back();
    return top;
}

Card* CardDeck::DeckPeekTop() const
{
    if (cards.empty()) {
        return nullptr;
    }
    return cards.back();
}

void CardDeck::DeckInsertRandom(Card* card)
{
    if (card == nullptr) {
        return;
    }

    if (cards.empty()) {
        cards.push_back(card);
        return;
    }

    std::mt19937_64& rng = Engine::EngineGet().EngineGetRNG();
    std::uniform_int_distribution<size_t> dist(0, cards.size());
    size_t pos = dist(rng);

    cards.insert(cards.begin() + pos, card);
}
