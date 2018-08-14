#include "SplashScreen.h"
#include "Game.h"
#include "EntityManager.h"
#include "SignalManager.h"
#include "InputManager.h"
#include "ResHelper.h"

SplashScreen::SplashScreen() {}

SplashScreen::~SplashScreen() {

}

void SplashScreen::show() {
    printf("SplashScreen show\n");

    SignalManager::fireGlobal(Signal::ASSETS_LOADED);
}

void SplashScreen::destroy() {
    // release resources
    Game::assetManager->unloadAssets("SplashScreen");

    // deletes entities
    EM->clearEntities();
    GI->signalManager->clearObservers();
    IM->clearObservers();
}
