#ifndef NORENGINE_TEXTURE_H
#define NORENGINE_TEXTURE_H


#include <SDL_gpu.h>
#include <vec2.hpp>

struct Texture {
    float u1, v1, u2, v2;
    GPU_Rect *bounds;

    GPU_Image *rawImage;
    glm::vec2 anchor;
    std::string name;

    Texture() {};

    Texture(GPU_Image *pImage, std::string pName, float x, float y, float w, float h) {
        name = pName;
        rawImage = pImage;
        u1 = x;
        v1 = y;
        u2 = x + w;
        v2 = y + h;

        bounds = new GPU_Rect {u1, v1, w, h};
    }

    ~Texture() {
//        SDL_Log("Texture dest: %s\n", name.c_str());
        GPU_FreeImage(rawImage);
    };
};


#endif //NORENGINE_TEXTURE_H
