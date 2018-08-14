#include "AiManager.h"

AiManager::AiManager() {
    init();
}

void AiManager::update(double dt) {
    // score hp
    // update enemy send interval
    if (Game::instance->elapsedTime > lastTime + interval) {
        lastTime = Game::instance->elapsedTime;
        createEnemy();

        interval -= dt;

        GI->gameConstants->enemy1v = (float) fmin((GI->gameConstants->enemy1v + 1 / dt), 0.8f);
        GI->gameConstants->enemy2v = (float) fmin((GI->gameConstants->enemy2v + 1 / dt), 0.9f);
        GI->gameConstants->enemy3v = (float) fmin((GI->gameConstants->enemy3v + 1 / dt), 1.0f);

        if (interval < 200.0f) {
            interval = 200.0f;
        }
    }
}

void AiManager::createEnemy() {
    int enemy = std::rand() % 4;

    glm::vec4 bounds = getEnemyStartingPoint();

    if (enemy == 0) {
        auto tc = Game::entityManager->getComponent<TransformComponent>(Tag::PLAYER, ComponentType::TRANSFORM);
        EM->create("Enemy", bounds, Tag::ENEMY_TAG)
                .texture("enemyship1")
                .component(BODY).component(ENEMY)
                .bodyOffset({10, -10, -20, -20})
                .motion({tc->originX(), tc->originY()}, 0.3f)
                .enemy(ENEMY_1);
    } else if (enemy == 1) {
        auto tc = Game::entityManager->getComponent<TransformComponent>(Tag::PLAYER, ComponentType::TRANSFORM);
        EM->create("Enemy", bounds, Tag::ENEMY_TAG)
                .texture("enemyship2")
                .component(BODY).component(ENEMY)
                .bodyOffset({10, -10, -20, -20})
                .motion({tc->originX(), tc->originY()}, 0.35f)
                .enemy(ENEMY_2);
    } else {
        auto tc = Game::entityManager->getComponent<TransformComponent>(Tag::PLAYER, ComponentType::TRANSFORM);
        EM->create("Enemy", bounds, Tag::ENEMY_TAG)
                .texture("enemyship3")
                .component(BODY).component(ENEMY)
                .bodyOffset({10, -10, -20, -20})
                .motion({tc->originX(), tc->originY()}, 0.45f)
                .enemy(ENEMY_3);
    }
}

glm::vec4 AiManager::getEnemyStartingPoint() {
    float x = 0;
    float y = 0;

    switch (std::rand() % 4) {
        case 0: // top
            y = 0;
            x = std::rand() % Game::instance->virtualWidth;
            break;
        case 1: // bottom
            y = Game::instance->virtualHeight + 10;
            x = std::rand() % Game::instance->virtualWidth;
            break;
        case 2: // right
            y = std::rand() % Game::instance->virtualHeight;
            x = Game::instance->virtualWidth;
            break;
        case 3: // left
            y = std::rand() % Game::instance->virtualHeight;
            x = -10;
            break;
        default:
            break;
    }

    return glm::vec4(x, y, -1, -1);
}

void AiManager::init() {
    lastTime = -100000;
    interval = 2000;
}
