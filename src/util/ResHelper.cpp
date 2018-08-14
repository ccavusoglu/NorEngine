#include "ResHelper.h"
#include "Game.h"

float ResHelper::getMidX(float w) {
    return Game::instance->virtualWidth / 2 - w / 2;
}

float ResHelper::getMidY(float h) {
    return Game::instance->virtualHeight / 2 + h / 2;
}

glm::vec2 ResHelper::getAlignedTextCoords(float x, float y, float w, float h, float textW, float textH, Align align) {
    glm::vec2 coords;

    switch (align) {
        case LEFT:
            coords.y = y - textH - h / 2;
            break;
        case RIGHT:
            coords.x = x - textW;
            coords.y = y - textH - h / 2;
            break;
        case CENTER:
            coords.x = x + (w) / 2;
            coords.y = y - (h + textH) / 2;
            break;
    }

    return coords;
}

glm::vec2 ResHelper::getAnchoredCoords(glm::vec4 bounds, glm::vec2 anchor) {
    glm::vec2 coords;

    coords.x = bounds.x + bounds.z / 2 - (0.5f - anchor.x) * bounds.z;
    coords.y = bounds.y - bounds.w / 2 - (0.5f - anchor.y) * bounds.w;

    return coords;
}
