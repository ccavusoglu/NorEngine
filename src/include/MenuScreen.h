#ifndef NORENGINE_MENUSCREEN_H
#define NORENGINE_MENUSCREEN_H


#include "Screen.h"

class MenuScreen : public Screen {
public:
    MenuScreen();

    ~MenuScreen() override;

    void show() override;

    void destroy() override;
};


#endif //NORENGINE_MENUSCREEN_H
