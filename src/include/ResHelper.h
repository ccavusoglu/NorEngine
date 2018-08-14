#ifndef NORENGINE_RESOLUTIONHELPER_H
#define NORENGINE_RESOLUTIONHELPER_H


#include "vec2.hpp"
#include "Component.h"

class ResHelper {
public:
    static float getMidX(float w);

    static float getMidY(float h);

    static glm::vec2 getAlignedTextCoords(float x, float y, float w, float h, float textW, float textH, Align align);

    static glm::vec2 getAnchoredCoords(glm::vec4 bounds, glm::vec2 anchor);
};


#endif //NORENGINE_RESOLUTIONHELPER_H
