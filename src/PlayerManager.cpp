#include "PlayerManager.h"
#include "Definitions.h"
#include "Game.h"

PlayerManager::PlayerManager() {
    playerHp = GC->initialHp;
    playerScore = GC->initialScore;
}

float PlayerManager::getPlayerHpPercentage() {
    return playerHp / GC->initialHp;
}

void PlayerManager::init() {
    playerHp = GC->initialHp;
    playerScore = GC->initialScore;
}
