#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

enum class EventType {
    DayChanged,
    MapSwitched,
    QuestStateChanged,
    WeatherChanged
};

struct DayChangedEvent {
    int year;
    int month;
    int day;
};

struct MapSwitchedEvent {
    std::string mapName;
};

// 前置声明以避免头文件循环
enum class QuestType;
enum class QuestState;
class Weather;

struct QuestStateChangedEvent {
    QuestType type;
    QuestState state;
    int currentAmount;
    int targetAmount;
};

struct WeatherChangedEvent {
    Weather* weather;
};

struct Event {
    EventType type;
    const void* data;
};

struct EventTypeHasher {
    size_t operator()(const EventType& type) const {
        return static_cast<size_t>(type);
    }
};

class EventBus {
public:
    using EventCallback = std::function<void(const Event&)>;

    static EventBus& getInstance();

    // 返回订阅ID，便于后续注销
    int subscribe(EventType type, EventCallback callback);
    void unsubscribe(EventType type, int id);
    void publish(const Event& event);

private:
    struct ListenerEntry {
        int id;
        EventCallback callback;
    };

    EventBus() = default;
    std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHasher> listeners;
    int nextId = 1;
};
