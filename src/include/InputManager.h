#ifndef NORENGINE_INPUTMANAGER_H
#define NORENGINE_INPUTMANAGER_H


#include <functional>
#include <unordered_map>
#include "Component.h"
#include "vec4.hpp"

enum InputEvent {
    TOUCH_DOWN,
    TOUCH_UP,
    TOUCH_DRAGGED
};


class Game;

class InputManager {

    std::unordered_multimap<int32_t, std::function<bool(int32_t e, float x, float y)>> observers;
    std::unordered_multimap<int32_t, std::function<bool(int32_t e, float x, float y)>> observersGlobal;

public:
    InputManager();

    void registerEvent(int32_t eventType, std::function<bool(int32_t e, float x, float y)> callback);

    void registerEventGlobal(int32_t eventType, std::function<bool(int32_t, float, float)> callback);

    void unregister(int32_t eventType);

    void clearObservers();

    void handleTouchEvent(InputEvent eventType, SDL_Event event);

    void handleKeyboardEvent(SDL_Keycode key);

    void addDefaultEventListeners();

private:
    void fire(int32_t eventType, float x, float y);

    void fireGlobal(InputEvent event, float x, float y);
};


#endif //NORENGINE_INPUTMANAGER_H
