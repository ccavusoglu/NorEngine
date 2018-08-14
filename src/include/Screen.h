#ifndef NORENGINE_SCREEN_H
#define NORENGINE_SCREEN_H


#include <cstdio>

enum ScreenType {
    SPLASH,
    MENU,
    GAME
};

struct ScreenManager;

class Screen {
public:
    ScreenManager *screenManager;

    virtual void show() = 0;

    virtual void destroy() = 0;

    virtual ~Screen() {}
};


#endif //NORENGINE_SCREEN_H
