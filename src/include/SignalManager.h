#ifndef NORENGINE_EVENTMANAGER_H
#define NORENGINE_EVENTMANAGER_H


#include <unordered_map>
#include "EntityManager.h"
#include "Definitions.h"
#include "Game.h"
#include "GameScreen.h"
#include "MenuScreen.h"

enum Signal {
    SEC_PASSED_1,
    ASSETS_LOADED,
    GAME_START,
    INCREASE_SCORE,
    DECREASE_HP,
    PLAYER_HIT,
    PROJECTILE_HIT
};

struct SignalArgs {
    float x, y;

    EntityWrapper source, target;

    SignalArgs() {}

    SignalArgs(float x, float y) : x(x), y(y) {}

    SignalArgs(EntityWrapper source, EntityWrapper target) : source(source), target(target) {}
};

struct SignalManager {
    bool eventsHalted = false;

    std::unordered_multimap<int, EntityWrapper> observers;

    static void registerEvent(Signal signal, EntityWrapper entity);

    static void registerEvent(Signal signal, std::string entity);

    static void fireGlobal(Signal signal);

    static void fireGlobal(Signal signal, SignalArgs signalArgs);

    static void fire(Signal signal);

    static void fire(Signal signal, SignalArgs signalArgs);

    void handleDecreaseHp(EntityWrapper entity, EntityWrapper src, EntityWrapper target);

    void handlePlayerHit(SignalArgs signalArgs);

    void handleEvent(Signal signal, EntityWrapper entity, SignalArgs signalArgs);

    void handleAssetsLoaded();

    void clearObservers();

    void handleIncreaseScore(EntityWrapper i);

    void handle1SecondPassed(EntityWrapper i);
};


#endif //NORENGINE_EVENTMANAGER_H
