#include "SDL.h"
#include "Game.h"

int main(int argc, char *args[]) {
    Game game(540, 960);
    game.initGame();

    try {
        game.run();
    } catch (const std::exception &e) {
        printf("WTF?: %s\n", e.what());
    }

    return 0;
}

