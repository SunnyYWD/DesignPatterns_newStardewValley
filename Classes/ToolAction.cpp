#include "ToolAction.h"
#include "Player.h"
#include "CropManager.h"
#include "FishingSystem.h"
#include "GameScene.h"
#include "Tree.h"
#include "Ore.h"

using namespace cocos2d;

void ShovelAction::execute(Player* player, const Vec2& clickPos) {
    if (!player) return;
    CropManager::getInstance()->onMouseDown(clickPos, player);
    player->beginActionWithOffset(player->getToolOffset(Player::ToolType::SHOVEL));
}

void WateringAction::execute(Player* player, const Vec2& clickPos) {
    if (!player) return;
    CropManager::getInstance()->onMouseDown(clickPos, player);
    player->beginActionWithOffset(player->getToolOffset(Player::ToolType::WATERING));
}

void AxeAction::execute(Player* player, const Vec2&) {
    if (!player) return;
    auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
    if (!scene) return;

    Vec2 playerPos = player->getPosition();
    for (auto tree : scene->trees) {
        Vec2 treePos = tree->getPosition();
        float distance = playerPos.distance(treePos);
        if (distance < 100.0f && tree->canBeChopped()) {
            auto chopEffect = Sprite::create("effects/chop_effect.png");
            if (chopEffect) {
                chopEffect->setPosition(tree->getPosition());
                scene->addChild(chopEffect, 10);
                auto fadeOut = FadeOut::create(0.3f);
                auto removeEffect = RemoveSelf::create();
                chopEffect->runAction(Sequence::create(fadeOut, removeEffect, nullptr));
            }
            tree->chop(10);
            player->beginActionWithOffset(player->getToolOffset(Player::ToolType::AXE));
            return;
        }
    }
}

void PickAction::execute(Player* player, const Vec2&) {
    if (!player) return;
    auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
    if (!scene) return;

    Vec2 playerPos = player->getPosition();
    for (auto ore : scene->ores) {
        Vec2 orePos = ore->getPosition();
        float distance = playerPos.distance(orePos);
        if (distance < 100.0f && ore->canBeDug()) {
            auto digEffect = Sprite::create("effects/dig_effect.png");
            if (digEffect) {
                digEffect->setPosition(ore->getPosition());
                scene->addChild(digEffect, 10);
                auto fadeOut = FadeOut::create(0.3f);
                auto removeEffect = RemoveSelf::create();
                digEffect->runAction(Sequence::create(fadeOut, removeEffect, nullptr));
            }
            ore->dig(5);
            player->beginActionWithOffset(player->getToolOffset(Player::ToolType::PICK));
            return;
        }
    }
}

void RodAction::execute(Player* player, const Vec2& clickPos) {
    if (!player) return;
    auto fishingSystem = FishingSystem::getInstance();
    if (fishingSystem->isCurrentlyFishing()) {
        fishingSystem->finishFishing();
        return;
    }
    if (fishingSystem->canFish(clickPos, player)) {
        fishingSystem->startFishing();
    }
}
