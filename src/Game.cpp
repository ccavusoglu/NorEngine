#include <ScreenManager.h>
#include "Game.h"
#include "InputManager.h"
#include "AiManager.h"
#include "SignalManager.h"
#include "SplashScreen.h"

AssetManager *Game::assetManager;
EntityManager *Game::entityManager;
InputManager *Game::inputManager;
PlayerManager *Game::playerManager;
EnemyManager *Game::enemyManager;
SignalManager *Game::signalManager;
GameConstants *Game::gameConstants;
Game *Game::instance;

Game::Game(uint16_t w, uint16_t h) : screenWidth(w), screenHeight(h) {
    gameRunning = true;
    instance = this;
    currentScreen = nullptr;
}

Game::~Game() {
    delete currentScreen;
    delete assetManager;
    delete entityManager;
    delete inputManager;
    delete aiManager;
    delete playerManager;
    delete enemyManager;
    delete signalManager;
    delete gameConstants;
    delete screenManager;
}

void Game::initGame() {
    printf("Game init\n");
    Util::initTime = std::chrono::high_resolution_clock::now();

    GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
    auto *targetWindow = GPU_Init(screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    SDL_GL_SetSwapInterval(1);
    GPU_SetVirtualResolution(targetWindow, virtualWidth, virtualHeight);

    // create managers
    gameConstants = new GameConstants();
    assetManager = new AssetManager();
    entityManager = new EntityManager();
    renderer.init(targetWindow);
    inputManager = new InputManager();
    aiManager = new AiManager();
    playerManager = new PlayerManager();
    enemyManager = new EnemyManager();
    signalManager = new SignalManager();
    screenManager = new ScreenManager();

    // add assets that should be loaded
    assetManager->addAssets();
    assetManager->loadAssetsAsync();

    // create initial screen
    setScreen(new SplashScreen(), ScreenType::SPLASH);
}

void Game::quit() {
    printf("Game quit!\n");
    GPU_Quit();
}

void Game::run() {
    float accumulatedTime = 0.0f;
    double lastTime = 0.0;
    uint32_t frameCount = 0;

    while (gameRunning) {
        auto timePointStart = std::chrono::high_resolution_clock::now();

        accumulatedTime += deltaTime;
        elapsedTime += deltaTime;

        if (elapsedTime - lastTime >= 1000) {
#ifdef WIN32
            if (screenManager->checkScreenFileChange(currentScreenType)) resetScreen();
#endif
            // one second passed
            fpsTemp += frameCount;
            frameCount = 0;
            lastTime = elapsedTime;
            seconds = (uint32_t) nearbyint(elapsedTime / 1000.0f);

            // every ~2 seconds
            auto interval = 2;
            if (seconds % interval == 0) {
                fps = fpsTemp / interval;
//                SDL_Log("%f\tFPS: %d FPS_TEMP: %d ELAPSED: %f SECONDS: %d Dt: %f\n", Util::getTimeStamp(), fps, fpsTemp, elapsedTime, seconds, deltaTime);
                fpsTemp = 0;
            }

            signalManager->fireGlobal(SEC_PASSED_1);
        }

        // diagnostic for spikes
        if (deltaTime > tickMs * 2) {
            SDL_Log("%f\tFPS: %d Frame: %llu Dt: %f\n", Util::getTimeStamp(), fps, frameId, deltaTime);
        }

        if (accumulatedTime >= tickMs) {
            pollEvents();

            while (accumulatedTime >= tickMs) {
                if (!gamePaused && currentScreenType == GAME) {
                    // systems/managers update
                    aiManager->update(tickMs);
                    entityManager->update(tickMs * speedMultiplier);
                }

                accumulatedTime -= tickMs;
            }
        } else {
            // in case v-sync disabled, slow down
            SDL_Delay(1);
        }

        renderer.render(deltaTime);

        auto timePointEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> duration = timePointEnd - timePointStart;

        deltaTime = duration.count();
        frameId++;
        frameCount++;
    }

    quit();
}

void Game::pause() {
    SDL_Log("Pause Game!\n");
    gamePaused = true;
}

void Game::resume() {
    SDL_Log("Resume Game!\n");
    gamePaused = false;
}

void Game::setScreen(Screen *screen, ScreenType type) {
    currentScreenType = type;

    if (currentScreen != nullptr) {
        currentScreen->destroy();
        delete currentScreen;
    }

    screen->screenManager = screenManager;
    currentScreen = screen;
    screenManager->prepareScreen(type);
    currentScreen->show();
}

void Game::resetScreen() {
    currentScreen->destroy();
    screenManager->prepareScreen(currentScreenType);
    currentScreen->show();
}

void Game::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                SDL_Log("stop game: %d\n", event.type);
                gameRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        if (currentScreenType == MENU) {
                            SDL_Log("stop game escape: %d\n", SDLK_ESCAPE);
                            gameRunning = false;
                        } else {
                            setScreen(new MenuScreen(), MENU);
                        }
                        break;
                    case SDLK_0:
                        SDL_Log("%f\tKEY NOT HANDLED: %d\n", Util::getTimeStamp(), event.key.keysym.sym);
                        break;
                    default:
                        inputManager->handleKeyboardEvent(event.key.keysym.sym);
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                inputManager->handleTouchEvent(InputEvent::TOUCH_DOWN, event);
                break;
            case SDL_MOUSEBUTTONUP:
                inputManager->handleTouchEvent(InputEvent::TOUCH_UP, event);
                break;
            case SDL_MOUSEMOTION:
                inputManager->handleTouchEvent(InputEvent::TOUCH_DRAGGED, event);
                break;
            default:
//                SDL_Log("%f\tEVENT NOT HANDLED: %d\n", Util::getTimeStamp(), event.type);
                break;
        }
    }
}

void Game::startGame() {
    playerManager->init();
    gameConstants->loadGameConstants();
    aiManager->init();
    gamePaused = false;
}
