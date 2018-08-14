#ifndef NORENGINE_GAME_H
#define NORENGINE_GAME_H


#include <chrono>
#include <memory>
#include <SDL_gpu.h>
#include "AssetManager.h"
#include "Entity.h"
#include "Texture.h"
#include "Component.h"
#include "Renderer.h"
#include "Screen.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "GameConstants.h"

class EntityManager;
class InputManager;
class AiManager;
class SignalManager;

class Game {
    const uint16_t screenWidth, screenHeight;

    Renderer renderer;
    bool gameRunning;
    ScreenManager *screenManager;

public:
    static AssetManager *assetManager;
    static EntityManager *entityManager;
    static InputManager *inputManager;
    static PlayerManager *playerManager;
    static EnemyManager *enemyManager;
    static SignalManager *signalManager;
    static GameConstants *gameConstants;
    static Game *instance;
    AiManager *aiManager;

    const uint16_t virtualWidth = 1080, virtualHeight = 1920;

    Screen *currentScreen;
    ScreenType currentScreenType;
    float targetFps = 60; // will be dynamically adjusted
    float tickMs = 1000 / targetFps;
    float speedMultiplier = 1;
    uint64_t frameId = 0;
    uint32_t fps = targetFps;
    uint32_t fpsTemp = 0;
    uint32_t seconds = 0;
    double deltaTime = 0.0;
    double elapsedTime = 0.0;
    bool gamePaused;

    Game(uint16_t w, uint16_t h);

    ~Game();

    void initGame();

    void setScreen(Screen *screen, ScreenType type);

    void startGame();

    void run();

    void pause();

    void resume();

private:
    void quit();

    void pollEvents();

    void resetScreen();
};


#endif //NORENGINE_GAME_H
