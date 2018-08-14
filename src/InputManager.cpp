#include "InputManager.h"
#include "Game.h"
#include "EntityManager.h"
#include "AiManager.h"

InputManager::InputManager() {
    addDefaultEventListeners();
}

void InputManager::handleTouchEvent(InputEvent eventType, SDL_Event event) {
    std::vector<InputComponent *> inputs = Game::entityManager->getComponents<InputComponent>(ComponentType::INPUT_C);
    float x, y;

    switch (eventType) {
        case TOUCH_DOWN:
        case TOUCH_UP:
            GPU_GetVirtualCoords(GPU_GetContextTarget(), &x, &y, event.button.x, event.button.y);
            break;
        case TOUCH_DRAGGED:
            GPU_GetVirtualCoords(GPU_GetContextTarget(), &x, &y, event.motion.x, event.motion.y);
            break;
        default:
            x = 0;
            y = 0;
    }

//    printf("%d SDL_MouseButtonEvent: %d %d : %f %f\n", event.type, event.x, event.y, x, y);

    bool handled = false;
    for (int i = 0; i < inputs.size(); ++i) {
        if (handled) break;
        auto tc = EM->getComponent<TransformComponent>(inputs[i]->owner, ComponentType::TRANSFORM);

        auto touched = &inputs[i]->touchDownCoords;

        switch (eventType) {
            case TOUCH_DOWN:
                if (inputs[i]->touchDownCallback != nullptr &&
                    Util::isIntersectRect(tc->rect.x + inputs[i]->bounds.x,
                                          tc->rect.y + inputs[i]->bounds.y,
                                          tc->rect.w + inputs[i]->bounds.z,
                                          tc->rect.h + inputs[i]->bounds.w,
                                          x, y, 0, 0)) {
                    touched->x = x;
                    touched->y = y;
                    inputs[i]->touchDownTime = Util::getTimeStamp();
                    if (inputs[i]->touchDownCallback(eventType, x, y)) {
                        handled = true;
                        break;
                    }
                }
                break;
            case TOUCH_UP:
                if (inputs[i]->touchUpCallback != nullptr) {
                    inputs[i]->touchUpCallback(eventType, x, y);
                }
                break;
            case TOUCH_DRAGGED: {
                auto dragTh = inputs[i]->dragThreshold;
                if (inputs[i]->touchDraggedCallback != nullptr &&
                    (x >= touched->x + dragTh.x || x <= touched->x - dragTh.x) &&
                    (y >= touched->y + dragTh.y || y <= touched->y - dragTh.y)) {
                    inputs[i]->touchDraggedCallback(eventType, x, y);
                }
            }
                break;
        }
    }

    if (!handled) {
        fire(eventType, x, y);
    }

    fireGlobal(eventType, x, y);
}

void InputManager::handleKeyboardEvent(SDL_Keycode key) {
    fire(key, 0, 0);
}

void InputManager::registerEvent(int32_t eventType, std::function<bool(int32_t e, float x, float y)> callback) {
    observers.insert({eventType, callback});
}

void InputManager::registerEventGlobal(int32_t eventType, std::function<bool(int32_t e, float x, float y)> callback) {
    observersGlobal.insert({eventType, callback});
}

void InputManager::fire(int32_t eventType, float x, float y) {
    auto pairs = observers.equal_range(eventType);
    for (auto it = pairs.first; it != pairs.second; ++it) {
        it->second(eventType, x, y);
    }
}

void InputManager::unregister(int32_t eventType) {
}

void InputManager::clearObservers() {
    observers.clear();
    observersGlobal.clear();
}

void InputManager::fireGlobal(InputEvent event, float x, float y) {
    auto pairs = observersGlobal.equal_range(event);
    for (auto it = pairs.first; it != pairs.second; ++it) {
        it->second(event, x, y);
    }
}

void InputManager::addDefaultEventListeners() {
    auto space = [](int32_t e, float x, float y) -> bool {
        auto tc = Game::entityManager->getComponents<TransformComponent>(ComponentType::TRANSFORM);
        auto entities = Game::entityManager->getEntities();

        printf("-------%d-I:%d/%d------\n", entities.size(), Game::entityManager->entitiesIndex.capacity(), Game::entityManager->entitiesIndex.size());
        for (int i = 0; i < tc.size(); ++i) {
            auto ent = Game::entityManager->getEntity(tc[i]->owner);
            printf("Entity: %s\tTag: %d\tx: %f y: %f\n", ent->name.c_str(), ent->tag, tc[i]->rect.x,
                   tc[i]->rect.y);
        }
        printf("-------%d------------\n", tc.size());

        return true;
    };

    auto backspace = [](int32_t e, float x, float y) -> bool {
        printf("---------------------\n");
        printf("%f\tDeleted Projectiles: %d\n", Util::getTimeStamp(), Game::entityManager->deleteEntity("Projectile"));
        printf("---------------------\n");

        return true;
    };

    auto debugOn = [](int32_t e, float x, float y) -> bool {
        std::cout << "Debug On" << std::endl;
        auto components = EM->getComponents<TransformComponent>(ComponentType::TRANSFORM);

        for (int i = 0; i < components.size(); ++i) {
            components[i]->isDebug = true;
        }

        return true;
    };

    auto debugOff = [](int32_t e, float x, float y) -> bool {
        std::cout << "Debug Off" << std::endl;
        auto components = EM->getComponents<TransformComponent>(ComponentType::TRANSFORM);

        for (int i = 0; i < components.size(); ++i) {
            components[i]->isDebug = false;
        }

        return true;
    };

    auto faster = [](int32_t e, float x, float y) -> bool {
        Game::instance->speedMultiplier *= 2;
        std::cout << "Faster: " << Game::instance->speedMultiplier << std::endl;
        return true;
    };

    auto slower = [](int32_t e, float x, float y) -> bool {
        Game::instance->speedMultiplier /= 2;
        std::cout << "Slower: " << Game::instance->speedMultiplier << std::endl;
        return true;
    };

    auto sendEnemy = [](int32_t e, float x, float y) -> bool {
        GI->aiManager->createEnemy();
        return true;
    };

    auto pause = [](int32_t e, float x, float y) -> bool {
        Game::instance->gamePaused ? Game::instance->resume() : Game::instance->pause();
        return true;
    };

    registerEvent(SDLK_SPACE, space);
    registerEvent(SDLK_DELETE, backspace);
    registerEvent(SDLK_d, debugOn);
    registerEvent(SDLK_s, debugOff);
    registerEvent(SDLK_UP, faster);
    registerEvent(SDLK_DOWN, slower);
    registerEvent(SDLK_c, sendEnemy);
    registerEvent(SDLK_p, pause);
}
