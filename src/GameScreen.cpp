#include "GameScreen.h"
#include "SignalManager.h"
#include "InputManager.h"
#include "ResHelper.h"

void GameScreen::show() {
    printf("GameScreen shown\n");

    auto playerInput = EM->getComponent<InputComponent>("Player", ComponentType::INPUT_C);
    playerInput->touchDownCallback = [](uint32_t type, float x, float y) -> bool {
        std::cout << Util::getTimeStamp() << "\t" << "Clicked On Player!" << std::endl;
        // shake

        return true;
    };

    auto playerControlInput = EM->getComponent<InputComponent>("PlayerControl", ComponentType::INPUT_C);
    setPlayerCallbacks(playerControlInput);

    SignalManager::registerEvent(Signal::DECREASE_HP, "HP_Bar");
    SignalManager::registerEvent(Signal::INCREASE_SCORE, "Score");

    IM->addDefaultEventListeners();
    Util::addDebugEntities();

    Game::instance->startGame();
}

void GameScreen::destroy() {
    // release resources
    Game::assetManager->unloadAssets("GameScreen");

    // deletes entities
    EM->clearEntities();
    GI->signalManager->clearObservers();
    IM->clearObservers();
}

GameScreen::~GameScreen() {
}

GameScreen::GameScreen() {
}

void GameScreen::setPlayerCallbacks(InputComponent *playerControlInput) {
    playerControlInput->touchDownCallback = [playerControlInput](uint32_t type, float x, float y) -> bool {
        playerControlInput->touchHold = true;

        // rotate player
        auto trc = EM->getComponent<TransformComponent>(Tag::PLAYER, ComponentType::TRANSFORM);
        auto angle = Util::getBearing(trc->originX(), trc->originY(), x, y);
        float dA = angle - trc->rotation;

        if (dA < 0) {
            dA += 360;
        }

        auto clockwise = true;
        if (dA > 180) {
            clockwise = false;
            dA = 360 - dA;
        }

        RotateBy::create(dA, 50, clockwise)->addToEntity(trc->owner);

        return true;
    };

    playerControlInput->touchUpCallback = [playerControlInput](uint32_t type, float x, float y) -> bool {
        if (!playerControlInput->touchHold) {
            return false;
        }

        playerControlInput->touchHold = false;

        // create projectile and fire
        auto tc = EM->getComponent<TransformComponent>(Tag::PLAYER, ComponentType::TRANSFORM);
        EM->create("Projectile", {tc->originX(), tc->originY(), -1, -1}, PROJECTILE)
                .texture("projectile_mothership", ZOrder::BACK)
                .transform({tc->originX(), tc->originY()}, tc->rotation)
                .component(BODY)
                .motion({0, 0}, Game::instance->gameConstants->playerProjectileV, false);

        return true;
    };

    playerControlInput->touchDraggedCallback = [playerControlInput](uint32_t type, float x, float y) -> bool {
        if (!playerControlInput->touchHold) {
            return false;
        }

        // rotate player
        auto trc = EM->getComponent<TransformComponent>(Tag::PLAYER, ComponentType::TRANSFORM);
        auto angle = Util::getBearing(trc->originX(), trc->originY(), x, y);
        float dA = angle;

        if (dA < 0) {
            dA += 360;
        }

        trc->setRotation(dA);

        return true;
    };
}
