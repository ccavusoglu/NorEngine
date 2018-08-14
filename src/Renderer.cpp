#include "Renderer.h"
#include "EntityManager.h"
#include "Game.h"
#include "ResHelper.h"

void Renderer::renderBegin() const {
    GPU_Clear(targetWindow);
}

void Renderer::renderEnd() const {
    GPU_Flip(targetWindow);
}

void Renderer::init(GPU_Target *pGPU_target) {
    targetWindow = pGPU_target;
    GPU_SetCamera(targetWindow, camera.getInternalCam());
    defaultFont = Game::assetManager->getDefaultFont();
    defaultFont->setDefaultColor(NFont::Color(0, 0, 255, 255));
    GPU_SetLineThickness(2);
}

void Renderer::render(double dt) {
    renderBegin();

    auto sprites = Game::entityManager->getSpriteComponents();
    auto texts = Game::entityManager->getComponents<TextComponent>(ComponentType::TEXT);

    for (int i = 0; i < sprites.size(); ++i) {
        Texture *texture = getSpriteTexture(sprites[i], dt);
        GPU_Image *image = texture->rawImage;
        auto tc = EM->getComponent<TransformComponent>(sprites[i]->owner, ComponentType::TRANSFORM);

        auto coord = ResHelper::getAnchoredCoords({tc->rect.x, tc->rect.y, tc->rect.w, tc->rect.h},
                                                  {texture->anchor.x, texture->anchor.y});

        // set different anchors for images in same sheet
        image->anchor_x = texture->anchor.x;
        image->anchor_y = texture->anchor.y;

        GPU_BlitTransform(image, texture->bounds, targetWindow,
                          coord.x, coord.y,
                          tc->rotation,
                          tc->scaleX, tc->scaleY);

        image->anchor_x = 0.5f;
        image->anchor_y = 0.5f;

        if (tc->isDebug) {
            drawDebug(tc, debugColor);

            GPU_Rectangle(targetWindow,
                          coord.x,
                          coord.y,
                          coord.x + tc->rect.w,
                          coord.y - tc->rect.h,
                          {55, 55, 55, 255});
        }
    }

    for (int j = 0; j < texts.size(); ++j) {
        auto tc = EM->getComponent<TransformComponent>(texts[j]->owner, ComponentType::TRANSFORM);

        if (texts[j]->isDirty) {
            const char *temp = texts[j]->text.c_str();
            texts[j]->w = defaultFont->getWidth("%s", temp);
            texts[j]->h = defaultFont->getHeight("%s", temp);
            texts[j]->isDirty = false;

            if (tc->rect.w == 0) tc->rect.w = texts[j]->w;
            if (tc->rect.h == 0) tc->rect.h = texts[j]->h;
        }

        glm::vec2 coords = ResHelper::getAlignedTextCoords(tc->rect.x, tc->rect.y, tc->rect.w, tc->rect.h,
                                                           texts[j]->w, texts[j]->h, texts[j]->align);

        defaultFont->draw(targetWindow,
                          coords.x, coords.y,
                          NFont::Effect(NFont::AlignEnum::CENTER,
                                        NFont::Scale(texts[j]->scale.x, texts[j]->scale.y),
                                        texts[j]->color),
                          "%s", texts[j]->text.c_str());

        if (tc->isDebug) {
            drawDebug(tc, fontDebugColor);
        }
    }

    renderEnd();
}

Renderer::Renderer() {
    GPU_Camera *pCamera = new GPU_Camera();
    pCamera->x = 0;
    pCamera->y = 0;
    pCamera->z = 0;
    pCamera->angle = 0;
    pCamera->zoom = 1;
    camera.setCamera(pCamera);
    debugColor = {0, 255, 0, 255};
    fontDebugColor = {0, 0, 255, 255};
}

void Renderer::setCamera(GPU_Camera *pCamera) {
    camera.setCamera(pCamera);
}

Renderer::~Renderer() {
    delete defaultFont;
}

void Renderer::drawDebug(TransformComponent *tc, SDL_Color color) {
    auto body = EM->getComponent<BodyComponent>(tc->owner, ComponentType::BODY);

    glm::vec4 bounds{tc->rect.x, tc->rect.y, tc->rect.w, tc->rect.h};
    if (body != nullptr) {
        bounds = body->getBounds(tc);
    }

    GPU_Rectangle(targetWindow,
                  bounds.x,
                  bounds.y,
                  bounds.x + bounds.z,
                  bounds.y - bounds.w,
                  color);
}

Texture *Renderer::getSpriteTexture(SpriteComponent *pComponent, double dt) {
    if (pComponent->animation == nullptr)
        return pComponent->texture;

    auto anim = pComponent->animation;

    anim->lastFrameTime += dt;

    if (anim->lastFrameTime > anim->interval) {
        anim->index += ((int) anim->lastFrameTime) / anim->interval;
        anim->lastFrameTime = 0;
        anim->index %= anim->animation.size();
    }

    return anim->animation[anim->index];
}
