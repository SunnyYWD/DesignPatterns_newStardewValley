#include "GameFacade.h"
#include "service/MapService.h"
#include "service/PlayerService.h"
#include "service/EntityService.h"
#include "service/UIService.h"
#include "service/EventService.h"
#include "service/AudioService.h"
#include "service/WeatherService.h"
#include "EventBus.h"
#include "GameMap.h"
#include "Player.h"
#include "Lewis.h"
#include "GameTime.h"
#include "LightManager.h"
#include "ItemSystem.h"
#include "FishingSystem.h"
#include "CropManager.h"
#include "cocos2d.h"

using namespace cocos2d;

GameFacade& GameFacade::instance() {
    static GameFacade inst;
    return inst;
}

void GameFacade::initialize(cocos2d::Scene* scene) {
    mapService = new MapService();
    playerService = new PlayerService();
    entityService = new EntityService();
    uiService = new UIService();
    eventService = new EventService();
    audioService = new AudioService();
    weatherService = new WeatherService();

    mapService->init(scene, "First");
    playerService->init(scene, mapService);
    entityService->init(scene, mapService, playerService);
    uiService->init(scene, playerService, mapService);
    eventService->init(scene, mapService, playerService);
    
    // 设置服务之间的依赖关系
    mapService->setAudioService(audioService);
    mapService->setUIService(uiService);
    mapService->setEventService(eventService);
    eventService->setUIService(uiService);
    eventService->setWeatherService(weatherService);
    
    weatherService->init();
    audioService->init();

    auto& bus = EventBus::getInstance();
    dayChangedSubscriptionId = bus.subscribe(EventType::DayChanged, [this](const Event& e) {
        const auto* payload = static_cast<const DayChangedEvent*>(e.data);
        if (payload && eventService) {
            eventService->onDayChanged(*payload);
        }
        if (payload && weatherService) {
            weatherService->onDayChanged(*payload);
        }
    });
    mapSwitchedSubscriptionId = bus.subscribe(EventType::MapSwitched, [this](const Event& e) {
        const auto* payload = static_cast<const MapSwitchedEvent*>(e.data);
        if (payload && entityService) {
            entityService->onMapChanged(payload->mapName);
        }
    });
    
    // 初始化物品系统（从GameScene迁移）
    ItemSystem* itemSystem = ItemSystem::getInstance();
    itemSystem->addItem("corn seed", 5);
    itemSystem->addItem("tomato seed", 5);
    itemSystem->addItem("fertilizer", 5);
}

void GameFacade::update(float dt) {
    // 更新游戏时间
    GameTime* gameTime = GameTime::getInstance();
    static int lastDay = gameTime->getDay();
    gameTime->update();
    
    // 检查日期变化
    if (gameTime->getDay() != lastDay) {
        onDayChanged();
        lastDay = gameTime->getDay();
    }
    
    // 更新各个服务
    weatherService->update(dt);
    audioService->update(dt);
    playerService->update(dt);
    entityService->update(dt);
    eventService->update(dt);
    uiService->update(dt);
    
    // 检查任务进度
    eventService->checkQuestProgress();
    
    // 更新地图相关（相机、前景遮挡等）
    mapService->postUpdate(dt, playerService);
    
    // 更新UI相机位置
    uiService->updateCameraAndUI();
    
    // 更新光照效果
    LightManager::getInstance()->update();
}

void GameFacade::switchMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos) {
    mapService->switchMap(mapName, targetTilePos, playerService);
}

void GameFacade::onDayChanged() {
    auto gameTime = GameTime::getInstance();
    DayChangedEvent payload{gameTime->getYear(), gameTime->getMonth(), gameTime->getDay()};
    EventBus::getInstance().publish(Event{EventType::DayChanged, &payload});
}

bool GameFacade::checkTransition(std::string& targetMap, cocos2d::Vec2& targetTilePos) {
    if (!mapService || !playerService) return false;
    
    GameMap* gameMap = mapService->getMap();
    Player* player = playerService->getPlayer();
    if (!gameMap || !player) return false;
    
    Vec2 playerTilePos = gameMap->convertToTileCoord(player->getPosition());
    TransitionInfo transition;
    if (gameMap->checkForTransition(playerTilePos, transition)) {
        targetMap = transition.targetMap;
        targetTilePos = transition.targetTilePos;
        return true;
    }
    return false;
}

Player* GameFacade::getPlayer() const { 
    return playerService ? playerService->getPlayer() : nullptr; 
}

GameMap* GameFacade::getMap() const { 
    return mapService ? mapService->getMap() : nullptr; 
}

// ========== 音频服务封装实现 ==========
void GameFacade::toggleMute() {
    if (audioService) {
        audioService->toggleMute();
    }
}

bool GameFacade::getIsMuted() const {
    return audioService ? audioService->getIsMuted() : false;
}

// ========== UI服务封装实现 ==========
void GameFacade::showQuestMark(cocos2d::Node* target) {
    if (uiService && target) {
        uiService->showQuestMark(target);
    }
}

void GameFacade::hideQuestMark() {
    if (uiService) {
        uiService->hideQuestMark();
    }
}

void GameFacade::setQuestTipLabelVisible(bool visible) {
    if (uiService) {
        uiService->setQuestTipLabelVisible(visible);
    }
}

void GameFacade::createQuestTipLabelIfNeeded() {
    if (uiService) {
        uiService->createQuestTipLabelIfNeeded();
    }
}

void GameFacade::updateQuestUI() {
    if (uiService) {
        uiService->updateQuestUI();
    }
}

void GameFacade::updateQuestUIPosition() {
    if (uiService) {
        uiService->updateQuestUIPosition();
    }
}

// ========== 事件服务封装实现 ==========
void GameFacade::handleQuestDialogue(Lewis* lewis) {
    if (eventService && lewis) {
        eventService->handleQuestDialogue(lewis);
    }
}

void GameFacade::checkQuestProgress() {
    if (eventService) {
        eventService->checkQuestProgress();
    }
}

// ========== 天气服务封装实现 ==========
void GameFacade::randomRefreshWeather() {
    if (weatherService) {
        weatherService->randomRefreshWeather();
    }
}

void GameFacade::setWeather(const std::string& type) {
    if (weatherService) {
        weatherService->setWeather(type);
    }
}
