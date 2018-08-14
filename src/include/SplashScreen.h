#ifndef NORENGINE_SPLASHSCREEN_H
#define NORENGINE_SPLASHSCREEN_H


#include "Screen.h"

class SplashScreen : public Screen {
public:
    SplashScreen();

    virtual ~SplashScreen();

    void show() override;

    void destroy() override;

private:
};


#endif //NORENGINE_SPLASHSCREEN_H
