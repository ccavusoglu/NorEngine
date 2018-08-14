#ifndef NORENGINE_AIMANAGER_H
#define NORENGINE_AIMANAGER_H


#include "Entity.h"
#include "Game.h"
#include "EntityManager.h"

class AiManager {
    double lastTime;

public:
    AiManager();

    void update(double dt);

    void createEnemy();

    void init();

private:
    double interval;

    glm::vec4 getEnemyStartingPoint();
};


#endif //NORENGINE_AIMANAGER_H
