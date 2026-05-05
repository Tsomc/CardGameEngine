#include <gtest/gtest.h>
#include "card.h"
#include "card_data.h"
#include "card_zone.h"
#include "card_deck.h"
#include "card_hand.h"
#include "card_battlefield.h"
#include "card_hand_layout.h"
#include <nlohmann/json.hpp>

class CardTest : public ::testing::Test {
protected:
    CardData tmpl;
    Card* card;

    void SetUp() override
    {
        tmpl.templateId = 1001;
        tmpl.name = "TestCard";
        tmpl.cost = 3;
        tmpl.textureCacheId = "test_card";
        tmpl.description = "A test card";
        tmpl.scriptPath = "";

        card = new Card(1, &tmpl);
        card->CardSetOwner(0);
    }

    void TearDown() override
    {
        delete card;
    }
};

TEST_F(CardTest, BasicProperties)
{
    EXPECT_EQ(card->CardGetInstanceId(), 1);
    EXPECT_EQ(card->CardGetData()->templateId, 1001);
    EXPECT_EQ(card->CardGetOwner(), 0);
    EXPECT_EQ(card->CardGetZone(), ZoneType::ZONE_DECK);
}

TEST_F(CardTest, MoveToZone)
{
    card->CardMoveToZone(ZoneType::ZONE_HAND);
    EXPECT_EQ(card->CardGetZone(), ZoneType::ZONE_HAND);
}

TEST_F(CardTest, FaceDown)
{
    EXPECT_FALSE(card->CardIsFaceDown());
    card->CardSetFaceDown(true);
    EXPECT_TRUE(card->CardIsFaceDown());
    card->CardSetFaceDown(false);
    EXPECT_FALSE(card->CardIsFaceDown());
}

TEST_F(CardTest, IntProperties)
{
    card->CardSetInt("attack", 5);
    card->CardSetInt("health", 10);
    EXPECT_EQ(card->CardGetInt("attack"), 5);
    EXPECT_EQ(card->CardGetInt("health"), 10);
    EXPECT_EQ(card->CardGetInt("nonexistent", -1), -1);
}

TEST_F(CardTest, Flags)
{
    card->CardSetFlags(0x01);
    EXPECT_TRUE(card->CardHasFlags(0x01));
    EXPECT_FALSE(card->CardHasFlags(0x02));
    card->CardSetFlags(0x02);
    EXPECT_TRUE(card->CardHasFlags(0x01));
    EXPECT_TRUE(card->CardHasFlags(0x02));
    card->CardClearFlags(0x01);
    EXPECT_FALSE(card->CardHasFlags(0x01));
    EXPECT_TRUE(card->CardHasFlags(0x02));
    card->CardClearAllFlags();
    EXPECT_EQ(card->CardGetAllFlags(), 0);
}

TEST_F(CardTest, Counters)
{
    EXPECT_EQ(card->CardGetCounter("+1/+1"), 0);
    card->CardAddCounter("+1/+1", 1);
    EXPECT_EQ(card->CardGetCounter("+1/+1"), 1);
    card->CardAddCounter("+1/+1", 2);
    EXPECT_EQ(card->CardGetCounter("+1/+1"), 3);
    card->CardSetCounter("+1/+1", 5);
    EXPECT_EQ(card->CardGetCounter("+1/+1"), 5);
    card->CardClearCounter("+1/+1");
    EXPECT_EQ(card->CardGetCounter("+1/+1", -1), -1);
}

TEST_F(CardTest, CustomData)
{
    nlohmann::json data = {{"type", "MINION"}, {"rarity", "COMMON"}};
    card->CardSetCustomData(data);
    EXPECT_EQ(card->CardGetCustomData()["type"], "MINION");
    EXPECT_EQ(card->CardGetCustomData()["rarity"], "COMMON");
}

TEST_F(CardTest, OwnerChange)
{
    card->CardSetOwner(1);
    EXPECT_EQ(card->CardGetOwner(), 1);
}

class CardZoneTest : public ::testing::Test {
protected:
    CardData tmpl1, tmpl2;
    CardZone zone;
    Card* card1;
    Card* card2;

    void SetUp() override
    {
        tmpl1.templateId = 1;
        tmpl2.templateId = 2;
        card1 = new Card(1, &tmpl1);
        card2 = new Card(2, &tmpl2);
    }

    void TearDown() override
    {
        delete card1;
        delete card2;
    }
};

TEST_F(CardZoneTest, AddAndCount)
{
    EXPECT_EQ(zone.ZoneCount(), 0);
    EXPECT_TRUE(zone.ZoneAdd(card1));
    EXPECT_EQ(zone.ZoneCount(), 1);
    EXPECT_TRUE(zone.ZoneAdd(card2));
    EXPECT_EQ(zone.ZoneCount(), 2);
}

TEST_F(CardZoneTest, Contains)
{
    zone.ZoneAdd(card1);
    EXPECT_TRUE(zone.ZoneContains(1));
    EXPECT_FALSE(zone.ZoneContains(2));
}

TEST_F(CardZoneTest, GetAndRemove)
{
    zone.ZoneAdd(card1);
    zone.ZoneAdd(card2);

    EXPECT_EQ(zone.ZoneGet(1), card1);
    EXPECT_EQ(zone.ZoneGetAt(0), card1);
    EXPECT_EQ(zone.ZoneGetAt(1), card2);

    EXPECT_TRUE(zone.ZoneRemove(1));
    EXPECT_EQ(zone.ZoneCount(), 1);
    EXPECT_EQ(zone.ZoneGetAt(0), card2);
}

TEST_F(CardZoneTest, Clear)
{
    zone.ZoneAdd(card1);
    zone.ZoneAdd(card2);
    EXPECT_EQ(zone.ZoneCount(), 2);
    zone.ZoneClear();
    EXPECT_EQ(zone.ZoneCount(), 0);
}

TEST_F(CardZoneTest, GetAll)
{
    zone.ZoneAdd(card1);
    zone.ZoneAdd(card2);
    auto all = zone.ZoneGetAll();
    EXPECT_EQ(all.size(), 2);
}

class CardDeckTest : public ::testing::Test {
protected:
    CardDeck deck;
    CardData tmpl;
    Card* card1;
    Card* card2;
    Card* card3;

    void SetUp() override
    {
        card1 = new Card(1, &tmpl);
        card2 = new Card(2, &tmpl);
        card3 = new Card(3, &tmpl);
        deck.ZoneAdd(card1);
        deck.ZoneAdd(card2);
        deck.ZoneAdd(card3);
    }

    void TearDown() override
    {
        delete card1;
        delete card2;
        delete card3;
    }
};

TEST_F(CardDeckTest, DrawTop)
{
    Card* drawn = deck.DeckDrawTop();
    ASSERT_NE(drawn, nullptr);
    EXPECT_EQ(drawn->CardGetInstanceId(), 3);
    EXPECT_EQ(deck.ZoneCount(), 2);
}

TEST_F(CardDeckTest, PeekTop)
{
    Card* peeked = deck.DeckPeekTop();
    ASSERT_NE(peeked, nullptr);
    EXPECT_EQ(peeked->CardGetInstanceId(), 3);
    EXPECT_EQ(deck.ZoneCount(), 3);
}

TEST_F(CardDeckTest, DrawFromEmpty)
{
    CardDeck emptyDeck;
    EXPECT_EQ(emptyDeck.DeckDrawTop(), nullptr);
}

class CardHandTest : public ::testing::Test {
protected:
    CardHand hand;
    CardData tmpl;
    Card* card1;
    Card* card2;

    void SetUp() override
    {
        hand.handMaxSize = 2;
        card1 = new Card(1, &tmpl);
        card2 = new Card(2, &tmpl);
    }

    void TearDown() override
    {
        delete card1;
        delete card2;
    }
};

TEST_F(CardHandTest, HandAdd)
{
    EXPECT_TRUE(hand.HandAdd(card1));
    EXPECT_FALSE(hand.HandIsFull());
    EXPECT_TRUE(hand.HandAdd(card2));
    EXPECT_TRUE(hand.HandIsFull());
}

class CardBattlefieldTest : public ::testing::Test {
protected:
    CardBattlefield bf;
    CardData tmpl;
    Card* card1;
    Card* card2;

    void SetUp() override
    {
        bf.battlefieldMaxSlots = 3;
        card1 = new Card(1, &tmpl);
        card2 = new Card(2, &tmpl);
    }

    void TearDown() override
    {
        delete card1;
        delete card2;
    }
};

TEST_F(CardBattlefieldTest, AddAndFull)
{
    EXPECT_FALSE(bf.BattlefieldIsFull());
    bf.BattlefieldAdd(card1, -1);
    EXPECT_FALSE(bf.BattlefieldIsFull());
    bf.BattlefieldAdd(card2, -1);
    EXPECT_FALSE(bf.BattlefieldIsFull());
}

class CardHandLayoutTest : public ::testing::Test {
protected:
    CardHandLayout layout;
    CardData tmpl;
    std::vector<Card*> cards;
    Card* card1;
    Card* card2;
    Card* card3;

    void SetUp() override
    {
        card1 = new Card(1, &tmpl);
        card2 = new Card(2, &tmpl);
        card3 = new Card(3, &tmpl);
        cards.push_back(card1);
        cards.push_back(card2);
        cards.push_back(card3);
    }

    void TearDown() override
    {
        delete card1;
        delete card2;
        delete card3;
    }
};

TEST_F(CardHandLayoutTest, Recalculate)
{
    Vec2 center(400.0f, 600.0f);
    auto poses = layout.CardHandRecalculate(cards, center, 200.0f, 3.14159f / 2.0f);
    EXPECT_EQ(poses.size(), 3);
}

TEST_F(CardHandLayoutTest, EmptyHand)
{
    std::vector<Card*> empty;
    auto poses = layout.CardHandRecalculate(empty, Vec2(0, 0), 100.0f, 1.0f);
    EXPECT_TRUE(poses.empty());
}

TEST_F(CardHandLayoutTest, SingleCard)
{
    std::vector<Card*> single = {card1};
    Vec2 center(400.0f, 600.0f);
    auto poses = layout.CardHandRecalculate(single, center, 200.0f, 1.0f);
    ASSERT_EQ(poses.size(), 1);
    EXPECT_FLOAT_EQ(poses[0].position.x, 400.0f);
    EXPECT_FLOAT_EQ(poses[0].position.y, 400.0f);
}

TEST_F(CardHandLayoutTest, HoverOffset)
{
    Vec2 center(400.0f, 600.0f);
    auto poses = layout.CardHandRecalculate(cards, center, 200.0f, 1.0f);
    Vec2 offset = layout.CardHandGetHoverOffset(poses[0], -30.0f);
    EXPECT_FLOAT_EQ(offset.x, poses[0].position.x);
    EXPECT_FLOAT_EQ(offset.y, poses[0].position.y - 30.0f);
}

TEST_F(CardHandLayoutTest, CalcAngleStep)
{
    float step = layout.CardHandCalcAngleStep(5, 100.0f, 200.0f, 80.0f);
    EXPECT_GT(step, 0.0f);
    EXPECT_LE(step, 80.0f / 200.0f);
}

TEST(CardDataTest, CustomDataAccess)
{
    CardData data;
    data.customData = {
        {"type", "MINION"},
        {"attack", 5},
        {"health", 3}
    };

    EXPECT_EQ(data.CardDataGetString("type"), "MINION");
    EXPECT_EQ(data.CardDataGetInt("attack"), 5);
    EXPECT_EQ(data.CardDataGetInt("health"), 3);
    EXPECT_EQ(data.CardDataGetInt("nonexistent", -1), -1);
    EXPECT_EQ(data.CardDataGetString("nonexistent", "default"), "default");
}
