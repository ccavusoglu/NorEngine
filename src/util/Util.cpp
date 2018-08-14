#include <cstring>
#include "Util.h"
#include "Game.h"
#include "EntityManager.h"
#include "InputManager.h"

std::chrono::high_resolution_clock::time_point Util::initTime;

float Util::getBearing(float x1, float y1, float x2, float y2) {
    float theta = std::atan2(x2 - x1, y1 - y2);
    if (theta < 0.0)
        theta += 2 * M_PI;

    return (float) (theta * 180 / M_PI);
}

bool Util::isIntersectRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return x2 <= x1 + w1 && x1 <= x2 + w2 && y2 - h2 <= y1 && y1 - h1 <= y2;
}

float Util::getTimeStamp() {
    auto timePointEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> duration = timePointEnd - initTime;

    float seconds = duration.count() / 1000;

    return seconds;
}

void Util::addDebugEntities() {
    auto coord = EM->create("Coords", glm::vec4{600, 100, 0, 0}, ANY)
            .text("", {0, 255, 100, 255}, {0.5f, 0.5f})
            .get();

    auto fps = EM->create("Fps", glm::vec4{540, 140, 0, 0}, ANY)
            .text("", {0, 255, 100, 255}, {0.5f, 0.5f})
            .get();

    auto coordCallback = [](int32_t event, float x, float y) -> bool {
        auto text = EM->getEntity("Coords")->getComponent<TextComponent>(TEXT);

        std::ostringstream str;
        str << "X: " << x << " Y: " << y;
        text->set(str.str());

        return false;
    };

    IM->registerEventGlobal(InputEvent::TOUCH_DRAGGED, coordCallback);
}

SDL_Color Util::getSDLColor(std::string color) {
    if (color.empty()) return {0, 255, 0, 255};

    int r, g, b, a;
    sscanf(color.c_str(), "%02x%02x%02x%02x", &r, &g, &b, &a);

    return {(Uint8) r, (Uint8) g, (Uint8) b, (Uint8) a};
}

int Util::getTag(const char *tag) {
    if (tag == nullptr) return ANY;

    if (!strcmp(tag, "PLAYER"))
        return PLAYER;
    else if (!strcmp(tag, "ENEMY_TAG"))
        return ENEMY_TAG;
    else if (!strcmp(tag, "PROJECTILE"))
        return PROJECTILE;
    else
        return ANY;
}

std::vector<std::string> Util::splitString(const char *str, const char *delimiter) {
    std::vector<std::string> temp;

    char *input = strdup(str);
    char *item;
    item = strtok(input, delimiter);
    while (item != nullptr) {
        temp.push_back(std::string(item));
        item = strtok(nullptr, delimiter);
    }

    free(input);
    return temp;
}
