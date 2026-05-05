#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <cstdint>

using EventHandle = uint32_t;
constexpr EventHandle INVALID_EVENT_HANDLE = 0;

struct Event {
    EventHandle eventType;
    float eventTimestamp;
};

class EventBus {
public:
    EventBus();
    ~EventBus();

    template<typename T>
    EventHandle EventSubscribe(std::function<void(const T&)> callback)
    {
        return DoSubscribe(typeid(T),
            [cb = std::move(callback)](const void* ev) {
                cb(*static_cast<const T*>(ev));
            });
    }

    template<typename T>
    bool EventUnsubscribe(EventHandle handle)
    {
        return DoUnsubscribe(std::type_index(typeid(T)), handle);
    }

    bool EventUnsubscribeAny(EventHandle handle);

    template<typename T>
    void EventEmit(const T& event)
    {
        DoEmit(typeid(T), &event);
    }

    template<typename T>
    void EventEmitDeferred(const T& event)
    {
        auto ptr = std::make_shared<T>(event);
        DoEmitDeferred(typeid(T), std::move(ptr));
    }

    void EventFlushDeferred();
    void EventClearDeferred();

    void ClearAll();

private:
    struct Subscription {
        EventHandle handle;
        std::function<void(const void*)> callback;
    };

    struct DeferredEntry {
        std::type_index typeIndex;
        std::shared_ptr<void> event;
    };

    EventHandle DoSubscribe(
        const std::type_info& type,
        std::function<void(const void*)> callback);

    bool DoUnsubscribe(const std::type_index& typeIndex, EventHandle handle);

    void DoEmit(const std::type_info& type, const void* event);

    void DoEmitDeferred(
        const std::type_info& type,
        std::shared_ptr<void> event);

    std::unordered_map<
        std::type_index,
        std::vector<Subscription>
    > immediate;

    std::vector<DeferredEntry> deferredQueue;
    std::unordered_map<EventHandle, std::type_index> handleToType;
    EventHandle nextHandle;
};

#endif
