#include "EnemyManager.h"
#include "EntityManager.h"
#include "GameConstants.h"
#include "Game.h"

void EnemyManager::initEnemy(EnemyComponent *enemyComponent, MotionComponent *motion) {
    switch (enemyComponent->type) {
        case ENEMY_1:
            enemyComponent->hp = GC->enemy1Hp;
            enemyComponent->hitPower = GC->enemy1HitPower;
            motion->velocity = GC->enemy1v;
            break;
        case ENEMY_2:
            enemyComponent->hp = GC->enemy2Hp;
            enemyComponent->hitPower = GC->enemy2HitPower;
            motion->velocity = GC->enemy2v;
            break;
        case ENEMY_3:
            enemyComponent->hp = GC->enemy3Hp;
            enemyComponent->hitPower = GC->enemy3HitPower;
            motion->velocity = GC->enemy3v;
            break;
    }
}
