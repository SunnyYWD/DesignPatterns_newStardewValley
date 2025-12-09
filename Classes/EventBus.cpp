#include "EventBus.h"
#include <algorithm>

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

int EventBus::subscribe(EventType type, EventCallback callback) {
    if (!callback) return -1;
    int id = nextId++;
    auto& vec = listeners[type];
    vec.push_back({id, std::move(callback)});
    return id;
}

void EventBus::unsubscribe(EventType type, int id) {
    if (id < 0) return;
    auto it = listeners.find(type);
    if (it == listeners.end()) return;
    auto& vec = it->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [id](const ListenerEntry& entry) {
        return entry.id == id;
    }), vec.end());
}

void EventBus::publish(const Event& event) {
    auto it = listeners.find(event.type);
    if (it == listeners.end()) return;
    // 复制一份，避免回调里增删订阅导致迭代问题
    auto listenersCopy = it->second;
    for (const auto& entry : listenersCopy) {
        if (entry.callback) {
            entry.callback(event);
        }
    }
}
