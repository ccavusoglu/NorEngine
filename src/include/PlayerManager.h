#ifndef NORENGINE_PLAYERMANAGER_H
#define NORENGINE_PLAYERMANAGER_H


struct PlayerManager {
    float playerHp;
    int playerScore;

    PlayerManager();

    void init();
    float getPlayerHpPercentage();
};


#endif //NORENGINE_PLAYERMANAGER_H
