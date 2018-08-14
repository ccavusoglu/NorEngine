#ifndef NORENGINE_RENDERER_H
#define NORENGINE_RENDERER_H


#include <SDL_gpu.h>
#include "Camera.h"
#include "Entity.h"
#include "NFont.h"

class Renderer {
    NFont *defaultFont;

public:
    Camera camera;
    Renderer();
    ~Renderer();

    void render(double dt);

    void renderBegin() const;

    void renderEnd() const;

    void init(GPU_Target *pGPU_target);

    void setCamera(GPU_Camera *camera);

private:
    GPU_Target *targetWindow;
    SDL_Color debugColor;
    SDL_Color fontDebugColor;

    void drawDebug(TransformComponent *pComponent, SDL_Color color);

    Texture *getSpriteTexture(SpriteComponent *pComponent, double dt);
};


#endif //NORENGINE_RENDERER_H
