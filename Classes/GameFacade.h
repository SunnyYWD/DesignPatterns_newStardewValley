#pragma once
#include "cocos2d.h"
#include "EventBus.h"
#include <string>

class GameMap;
class Player;
struct TransitionInfo;

class GameFacade {
public:
    static GameFacade& instance();

    void initialize(cocos2d::Scene* scene);
    void update(float dt);
    void switchMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos);
    
    // 日期变化处理
    void onDayChanged();
    
    // 检查传送点
    bool checkTransition(std::string& targetMap, cocos2d::Vec2& targetTilePos);
    
    // ========== 地图服务封装 ==========
    // 获取核心游戏对象
    Player* getPlayer() const;
    GameMap* getMap() const;
    
    // ========== 音频服务封装 ==========
    void toggleMute();
    bool getIsMuted() const;
    
    // ========== UI服务封装 ==========
    void showQuestMark(cocos2d::Node* target);
    void hideQuestMark();
    void setQuestTipLabelVisible(bool visible);
    void createQuestTipLabelIfNeeded();
    void updateQuestUI();
    void updateQuestUIPosition();
    
    // ========== 事件服务封装 ==========
    void handleQuestDialogue(class Lewis* lewis);
    void checkQuestProgress();
    
    // ========== 天气服务封装 ==========
    void randomRefreshWeather();
    void setWeather(const std::string& type); // "rain"/"drought"/"normal"

private:
    GameFacade() = default;
    ~GameFacade() = default;
    GameFacade(const GameFacade&) = delete;
    GameFacade& operator=(const GameFacade&) = delete;

    class MapService* mapService = nullptr;
    class PlayerService* playerService = nullptr;
    class EntityService* entityService = nullptr;
    class UIService* uiService = nullptr;
    class EventService* eventService = nullptr;
    class AudioService* audioService = nullptr;
    class WeatherService* weatherService = nullptr;
    int dayChangedSubscriptionId = -1;
    int mapSwitchedSubscriptionId = -1;
    
    // 内部服务访问器（仅供内部使用，不对外暴露）
    class UIService* getUIService() const { return uiService; }
    class AudioService* getAudioService() const { return audioService; }
    class EventService* getEventService() const { return eventService; }
};
