#include "event_bus.h"
#include <algorithm>
using namespace engine;
EventBus::EventBus()
    : nextHandle(1)
{
}

EventBus::~EventBus()
{
    ClearAll();
}

EventHandle EventBus::DoSubscribe(
    const std::type_info& type,
    std::function<void(const void*)> callback)
{
    EventHandle handle = nextHandle++;
    Subscription sub;
    sub.handle = handle;
    sub.callback = std::move(callback);
    std::type_index ti(type);
    immediate[ti].push_back(std::move(sub));
    handleToType.emplace(handle, ti);
    return handle;
}

bool EventBus::DoUnsubscribe(const std::type_index& typeIndex, EventHandle handle)
{
    auto it = immediate.find(typeIndex);
    if (it == immediate.end()) {
        return false;
    }

    auto& subs = it->second;
    auto subIt = std::find_if(subs.begin(), subs.end(),
        [handle](const Subscription& sub) { return sub.handle == handle; });

    if (subIt == subs.end()) {
        return false;
    }

    subs.erase(subIt);
    handleToType.erase(handle);

    if (subs.empty()) {
        immediate.erase(it);
    }

    return true;
}

bool EventBus::EventUnsubscribeAny(EventHandle handle)
{
    if (handle == INVALID_EVENT_HANDLE) {
        return false;
    }

    auto it = handleToType.find(handle);
    if (it == handleToType.end()) {
        return false;
    }

    return DoUnsubscribe(it->second, handle);
}

void EventBus::DoEmit(const std::type_info& type, const void* event)
{
    auto it = immediate.find(std::type_index(type));
    if (it == immediate.end()) {
        return;
    }

    for (auto& sub : it->second) {
        if (sub.callback) {
            sub.callback(event);
        }
    }
}

void EventBus::DoEmitDeferred(const std::type_info& type, std::shared_ptr<void> event)
{
    deferredQueue.push_back({std::type_index(type), std::move(event)});
}

void EventBus::EventFlushDeferred()
{
    auto queue = std::move(deferredQueue);
    for (auto& entry : queue) {
        auto it = immediate.find(entry.typeIndex);
        if (it == immediate.end()) {
            continue;
        }
        for (auto& sub : it->second) {
            if (sub.callback) {
                sub.callback(entry.event.get());
            }
        }
    }
}

void EventBus::EventClearDeferred()
{
    deferredQueue.clear();
}

void EventBus::ClearAll()
{
    immediate.clear();
    deferredQueue.clear();
    handleToType.clear();
}
