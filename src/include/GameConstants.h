#ifndef NORENGINE_GAMECONSTANTS_H
#define NORENGINE_GAMECONSTANTS_H


struct GameConstants {
    float initialHp;
    int initialScore;

    float playerProjectileV;

    float enemy1Hp;
    float enemy1HitPower;
    float enemy1v;

    float enemy2Hp;
    float enemy2HitPower;
    float enemy2v;

    float enemy3Hp;
    float enemy3HitPower;
    float enemy3v;

    GameConstants();

    void loadGameConstants();
};


#endif //NORENGINE_GAMECONSTANTS_H
