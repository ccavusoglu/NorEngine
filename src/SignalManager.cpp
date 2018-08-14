#include "SignalManager.h"

void printRegisteredEvents(std::unordered_multimap<int, EntityWrapper> multimap) {
    for (auto item : multimap) {
        std::cout << " First: " << item.first << " Second: " << item.second << std::endl;
    }
}

void SignalManager::registerEvent(Signal signal, EntityWrapper entity) {
    GI->signalManager->observers.insert({signal, entity});
}

void SignalManager::fire(Signal signal) {
    fire(signal, SignalArgs());
}

void SignalManager::fire(Signal signal, SignalArgs signalArgs) {
    // queue maybe?
    auto pairs = GI->signalManager->observers.equal_range(signal);
    for (auto it = pairs.first; it != pairs.second; ++it) {
        GI->signalManager->handleEvent(signal, it->second, signalArgs);
    }
}

void SignalManager::fireGlobal(Signal signal, SignalArgs signalArgs) {
    GI->signalManager->handleEvent(signal, 0, signalArgs);
}

void SignalManager::fireGlobal(Signal signal) {
    GI->signalManager->handleEvent(signal, 0, SignalArgs());
}

void SignalManager::handleEvent(Signal signal, EntityWrapper entity, SignalArgs signalArgs) {
    if (eventsHalted) {
        std::cout << "Events halted!: " << signal << " Entity: " << entity << std::endl;
        return;
    }

    switch (signal) {
        case ASSETS_LOADED:
            handleAssetsLoaded();
            break;
        case GAME_START:
            Game::instance->setScreen(new GameScreen(), ScreenType::GAME);
            break;
        case DECREASE_HP:
            handleDecreaseHp(entity, signalArgs.source, signalArgs.target);
            break;
        case INCREASE_SCORE:
            handleIncreaseScore(entity);
            break;
        case PLAYER_HIT:
            handlePlayerHit(signalArgs);
            break;
        case PROJECTILE_HIT:
            EM->deleteEntity(signalArgs.source);
            EM->deleteEntity(signalArgs.target);
            break;
        case SEC_PASSED_1:
            handle1SecondPassed(entity);
            break;
    }
}

void SignalManager::clearObservers() {
    observers.erase(observers.begin(), observers.end());
}

void SignalManager::handleIncreaseScore(EntityWrapper entity) {
    auto txc = EM->getComponent<TextComponent>(entity, ComponentType::TEXT);

    txc->set(Util::toString(Util::toFloat(txc->text) + 1));
}

void SignalManager::handleAssetsLoaded() {
    if (Game::instance->currentScreenType == ScreenType::SPLASH) {
        Game::instance->setScreen(new MenuScreen(), ScreenType::MENU);
    }
}

void SignalManager::handleDecreaseHp(EntityWrapper entity, EntityWrapper source, EntityWrapper target) {
    auto amount = EM->getComponent<EnemyComponent>(source, ComponentType::ENEMY)->hitPower;
    Game::playerManager->playerHp -= amount;

    auto percentage = Game::playerManager->getPlayerHpPercentage();

    auto tc = EM->getComponent<TransformComponent>(entity, ComponentType::TRANSFORM);
    auto sprite = EM->getComponent<SpriteComponent>(entity, ComponentType::SPRITE);
    tc->scaleX = (tc->rect.w * percentage) / sprite->texture->bounds->w;
}

void SignalManager::handlePlayerHit(SignalArgs signalArgs) {
    SignalManager::fire(Signal::DECREASE_HP, signalArgs);
    EM->deleteEntity(signalArgs.source);

    auto percentage = Game::playerManager->getPlayerHpPercentage();

    if (percentage <= 0) {
        // FINISHED
        Game::instance->setScreen(new MenuScreen(), ScreenType::MENU);
    }
}

void SignalManager::handle1SecondPassed(EntityWrapper entity) {
    auto ent = EM->getEntity("Fps");
    auto text = ent->getComponent<TextComponent>(TEXT);

    std::ostringstream str;
    str << "FPS: " << Game::instance->fps << " T: " << Game::instance->elapsedTime;

    text->set(str.str());
}

void SignalManager::registerEvent(Signal signal, std::string entity) {
    registerEvent(signal, EM->getEntity(entity)->id);
}
