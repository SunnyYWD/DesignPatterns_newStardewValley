#pragma once
#include "cocos2d.h"
#include <memory>
#include <unordered_map>

class Player;
class GameScene;

class IToolAction {
public:
    virtual ~IToolAction() = default;
    virtual void execute(Player* player, const cocos2d::Vec2& clickPos) = 0;
};

class ShovelAction : public IToolAction {
public:
    void execute(Player* player, const cocos2d::Vec2& clickPos) override;
};

class WateringAction : public IToolAction {
public:
    void execute(Player* player, const cocos2d::Vec2& clickPos) override;
};

class AxeAction : public IToolAction {
public:
    void execute(Player* player, const cocos2d::Vec2& clickPos) override;
};

class PickAction : public IToolAction {
public:
    void execute(Player* player, const cocos2d::Vec2& clickPos) override;
};

class RodAction : public IToolAction {
public:
    void execute(Player* player, const cocos2d::Vec2& clickPos) override;
};
