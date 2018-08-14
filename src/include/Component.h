#ifndef NORENGINE_COMPONENT_H
#define NORENGINE_COMPONENT_H


#include <functional>
#include <iostream>
#include <cmath>
#include "Texture.h"
#include "NFont.h"
#include "Util.h"
#include "Definitions.h"
#include "vec2.hpp"
#include "vec4.hpp"
#include <unordered_map>

struct Entity;

// TODO: enum class doesn't implicitly print itself as integer. thus removed...
enum ComponentType {
    BODY = 0,
    ENEMY,
    INPUT_C,
    MOTION,
    SPRITE,
    ANIMATION,
    TEXT,
    TRANSFORM
};

enum EnemyType {
    ENEMY_1,
    ENEMY_2,
    ENEMY_3
};

enum Align {
    LEFT,
    RIGHT,
    CENTER
};

enum ZOrder {
    BACKGROUND = -2,
    BACK = -1,
    DEFAULT = 0,
    TOP = 1,
    POPUP = 2
};

inline ComponentType operator|(ComponentType a, ComponentType b) {
    return static_cast<ComponentType>(static_cast<int>(a) | static_cast<int>(b));
}

struct Component {
    EntityWrapper owner;
    std::string ownerName = "";
    ComponentType type;
    bool isAlive;

    Component() = default;

    Component(const Component &component) {
        printf("Copied comp: %d\n", type);
    }

    virtual ~Component() {
//        printf("Component Dest: %d Owner: %s - %p\n", type, ownerName.c_str(), &*this);
    }
};

struct TransformComponent : public Component {
    GPU_Rect rect;
    float rotation;
    float scaleX, scaleY;

    bool isVisible;
    bool isTouchable;
    bool isDebug;

    TransformComponent() {
        type = ComponentType::TRANSFORM;
        rotation = 0.0f;
        scaleX = 1.0f;
        scaleY = 1.0f;
        isDebug = false;
    }

    TransformComponent(const TransformComponent &transformComponent) {
        SDL_Log("Copied comp: %d\n", type);
    }

    float originX() {
        return rect.x + rect.w / 2;
    }

    float originY() {
        return rect.y - rect.h / 2;
    }

    void setRotation(float d) {
        if (d < 0) d += 360;
        rotation = (float) std::fmod(d, 360);
    }

    void init(float pX, float pY, float pW, float pH) {
        rect.x = pX;
        rect.y = pY;
        rect.w = pW;
        rect.h = pH;
        isVisible = false;
        isTouchable = false;
    }

    void setXY(float x, float y) {
        rect.x = x;
        rect.y = y;
    }

    void translate(float x, float y) {
        rect.x += x;
        rect.y += y;
    }
};

struct InputComponent : public Component {
    std::function<bool(uint32_t e, float x, float y)> touchDownCallback = nullptr;
    std::function<bool(uint32_t e, float x, float y)> touchUpCallback = nullptr;
    std::function<bool(uint32_t e, float x, float y)> touchDraggedCallback = nullptr;
    std::function<bool(int32_t e, float x, float y)> keyCallback = nullptr;

    glm::vec4 bounds = {0, 0, 0, 0};

    bool touchHold = false;
    float touchDownTime = 0;
    glm::vec2 touchDownCoords = {-1, -1};
    glm::vec2 dragThreshold = {15, 15};

    InputComponent() {
        type = ComponentType::INPUT_C;
    }
};

struct MotionComponent : public Component {
    float velocity;
    float direction;
    bool targeted = false;
    float targetX, targetY;

    MotionComponent() {
        type = ComponentType::MOTION;
    }

    void init(TransformComponent *tc) {
        direction = tc->rotation;
    }
};

struct AnimationComponent {
    std::vector<Texture *> animation;
    int interval;
//    std::unordered_map<std::string, Texture **> animations;
//    std::unordered_map<std::string, int> intervals;
    int index = 0;
    double lastFrameTime = 0;
};

struct SpriteComponent : public Component {
    Texture *texture;
    ZOrder zOrder;
    AnimationComponent *animation{nullptr};

    SpriteComponent() : Component() {
        type = ComponentType::SPRITE;
    }

    SpriteComponent(const SpriteComponent &inputComponent) {
        printf("Copied comp: %d\n", type);
    }

    void initAnimation(AnimationComponent *pAnimations, glm::vec2 anchor, TransformComponent *tc, ZOrder order) {
        animation = pAnimations;
        zOrder = order;

        for (int i = 0; i < pAnimations->animation.size(); ++i) {
            pAnimations->animation[i]->anchor = anchor;
        }

        checkBounds(tc, pAnimations->animation[0]);
    }

    void initSprite(Texture *t, glm::vec2 anchor, TransformComponent *tc, ZOrder order) {
        texture = t;
        texture->anchor = anchor;
        zOrder = order;

        checkBounds(tc, t);
    }

    void checkBounds(TransformComponent *tc, Texture *t) {
        if (tc->rect.w == -1) {
            tc->rect.w = t->u2 - t->u1;
        } else {
            tc->scaleX = tc->rect.w / (t->u2 - t->u1);
        }

        if (tc->rect.h == -1) {
            tc->rect.h = t->v2 - t->v1;
        } else {
            tc->scaleY = tc->rect.h / (t->v2 - t->v1);
        }
    }

    virtual ~SpriteComponent() {
        if (animation != nullptr) {
            delete animation;
            animation = nullptr;
        }
    }
};

struct BodyComponent : public Component {
    glm::vec4 boundsOffset;

    BodyComponent() : Component() {
        type = ComponentType::BODY;
    }

    void init(glm::vec4 offset) {
        boundsOffset = offset;
    }

    glm::vec4 getBounds(TransformComponent *tc) {
        glm::vec4 bounds;

        if ((tc->rotation > 45 && tc->rotation < 135) || (tc->rotation > 225 && tc->rotation < 315)) {
            bounds.x = tc->originX() - tc->rect.h / 2 - boundsOffset.y;
            bounds.y = tc->originY() + tc->rect.w / 2 - boundsOffset.x;
            bounds.z = tc->rect.h + boundsOffset.w;
            bounds.w = tc->rect.w + boundsOffset.z;
        } else {
            bounds.x = tc->rect.x + boundsOffset.x;
            bounds.y = tc->rect.y + boundsOffset.y;
            bounds.z = tc->rect.w + boundsOffset.z;
            bounds.w = tc->rect.h + boundsOffset.w;
        }

        return bounds;
    }
};

struct TextComponent : public Component {
    std::string text;
    bool isDirty;
    float w, h;
    float dX, dY;
    glm::vec2 scale = {1, 1};
    SDL_Color color;
    Align align;

    TextComponent() : Component() {
        type = ComponentType::TEXT;
    }

    void init(std::string pText, SDL_Color pColor, Align pAlign, glm::vec2 pScale) {
        text = pText;
        isDirty = true;
        color = pColor;
        align = pAlign;
        scale = pScale;
    }

    void set(std::string pText) {
        text = pText;
        isDirty = true;
    }
};

struct EnemyComponent : public Component {
    EnemyType type;
    float hp;
    float firePower;
    float hitPower;
};


#endif //NORENGINE_COMPONENT_H
