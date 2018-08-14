#ifndef NORENGINE_ENEMYMANAGER_H
#define NORENGINE_ENEMYMANAGER_H


#include "Component.h"
#include "Entity.h"

struct EnemyManager {

    void initEnemy(EnemyComponent *enemyComponent, MotionComponent *motion);
};


#endif //NORENGINE_ENEMYMANAGER_H
