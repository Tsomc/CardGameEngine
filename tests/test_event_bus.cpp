#include <gtest/gtest.h>
#include "event_bus.h"

struct TestEvent1 : Event {
    int value;
};

struct TestEvent2 : Event {
    std::string message;
};

TEST(EventBusTest, SubscribeAndEmit)
{
    EventBus bus;
    int receivedValue = 0;

    bus.EventSubscribe<TestEvent1>(
        [&](const TestEvent1& e) {
            receivedValue = e.value;
        });

    TestEvent1 event;
    event.value = 42;
    bus.EventEmit(event);

    EXPECT_EQ(receivedValue, 42);
}

TEST(EventBusTest, MultipleSubscribers)
{
    EventBus bus;
    int count1 = 0, count2 = 0;

    bus.EventSubscribe<TestEvent1>([&](const TestEvent1&) { count1++; });
    bus.EventSubscribe<TestEvent1>([&](const TestEvent1&) { count2++; });

    TestEvent1 event;
    event.value = 0;
    bus.EventEmit(event);

    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 1);
}

TEST(EventBusTest, Unsubscribe)
{
    EventBus bus;
    int count = 0;

    EventHandle handle = bus.EventSubscribe<TestEvent1>(
        [&](const TestEvent1&) { count++; });

    TestEvent1 event;
    event.value = 0;
    bus.EventEmit(event);
    EXPECT_EQ(count, 1);

    bus.EventUnsubscribe<TestEvent1>(handle);
    bus.EventEmit(event);
    EXPECT_EQ(count, 1);
}

TEST(EventBusTest, DeferredEmit)
{
    EventBus bus;
    int receivedValue = 0;

    bus.EventSubscribe<TestEvent1>(
        [&](const TestEvent1& e) {
            receivedValue = e.value;
        });

    TestEvent1 event;
    event.value = 99;
    bus.EventEmitDeferred(event);

    EXPECT_EQ(receivedValue, 0);

    bus.EventFlushDeferred();
    EXPECT_EQ(receivedValue, 99);
}

TEST(EventBusTest, DifferentEvents)
{
    EventBus bus;
    int intVal = 0;
    std::string strVal;

    bus.EventSubscribe<TestEvent1>(
        [&](const TestEvent1& e) { intVal = e.value; });
    bus.EventSubscribe<TestEvent2>(
        [&](const TestEvent2& e) { strVal = e.message; });

    TestEvent1 e1; e1.value = 7;
    TestEvent2 e2; e2.message = "hello";

    bus.EventEmit(e1);
    bus.EventEmit(e2);

    EXPECT_EQ(intVal, 7);
    EXPECT_EQ(strVal, "hello");
}

TEST(EventBusTest, ClearDeferred)
{
    EventBus bus;
    int count = 0;

    bus.EventSubscribe<TestEvent1>([&](const TestEvent1&) { count++; });

    TestEvent1 event;
    event.value = 0;
    bus.EventEmitDeferred(event);
    bus.EventEmitDeferred(event);
    bus.EventEmitDeferred(event);

    bus.EventClearDeferred();
    bus.EventFlushDeferred();

    EXPECT_EQ(count, 0);
}
