#include "MenuScreen.h"
#include "Game.h"
#include "EntityManager.h"
#include "ScreenManager.h"
#include "InputManager.h"
#include "SignalManager.h"

MenuScreen::MenuScreen() {
}

MenuScreen::~MenuScreen() {

}

void MenuScreen::show() {
    printf("MenuScreen shown\n");

    auto input = EM->getComponent<InputComponent>("PlayButton", INPUT_C);

    input->touchDownCallback = [&](uint32_t eventType, float x, float y) -> bool {
        SignalManager::fireGlobal(Signal::GAME_START);

        return true;
    };

    IM->addDefaultEventListeners();
    Util::addDebugEntities();
}

void MenuScreen::destroy() {
    // release resources
    AM->unloadAssets("MenuScreen");

    // deletes entities
    EM->clearEntities();
    IM->clearObservers();
}

