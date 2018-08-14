#ifndef NORENGINE_SCREENMANAGER_H
#define NORENGINE_SCREENMANAGER_H


#include "Screen.h"
#include "tinyxml2.h"

constexpr char screensFile[]{"data/screens.xml"};

struct ScreenManager {
    void prepareScreen(ScreenType type);

    void createEntity(tinyxml2::XMLElement *pElement);

    bool checkScreenFileChange(ScreenType type);
};


#endif //NORENGINE_SCREENMANAGER_H
