#include "GameConstants.h"

GameConstants::GameConstants() {
    loadGameConstants();
}

void GameConstants::loadGameConstants() {
    initialHp = 100;
    initialScore = 0;

    playerProjectileV = 0.5f;

    enemy1Hp = 100;
    enemy1HitPower = 1.0f;
    enemy1v = 0.25f;

    enemy2Hp = 100;
    enemy2HitPower = 2.0f;
    enemy2v = 0.35f;

    enemy3Hp = 100;
    enemy3HitPower = 3.0f;
    enemy3v = 0.40f;
}
