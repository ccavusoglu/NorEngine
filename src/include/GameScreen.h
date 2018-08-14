#ifndef NORENGINE_GAMESCREEN_H
#define NORENGINE_GAMESCREEN_H


#include "Screen.h"
#include "Component.h"

class GameScreen : public Screen{

public:
    GameScreen();

    void show() override;

    void destroy() override;

    ~GameScreen() override;

    void setPlayerCallbacks(InputComponent *pComponent);
};


#endif //NORENGINE_GAMESCREEN_H
