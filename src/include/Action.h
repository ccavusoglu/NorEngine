#ifndef NORENGINE_ACTION_H
#define NORENGINE_ACTION_H


#include <functional>
#include <vector>
#include "Interpolation.h"
#include "Util.h"
#include "Component.h"

enum ActionType {
    MoveTo = 0,
    MoveBy,
    ScaleTo,
    ScaleBy,
    RotateTo,
    RotateBy,
    FadeIn,
    FadeOut
};

struct Action {
    EntityWrapper target;
    bool isDone;
    bool clockwise;
    double duration, elapsedTime, percent, lastPercent;
    float speed;
    bool began;
    std::function<void()> endCallback;
    std::function<double(double duration)> interpolation = Interpolation::linear;

    Action();

    virtual ~Action();

    virtual void addToEntity(EntityWrapper entity);

    void setSpeed(float s);

    virtual void begin();

    virtual void end();

    virtual bool step(double tickMs);
};

struct SetAction : public Action {
    std::vector<Action *> actions;

    SetAction *add(Action *action);

    void addToEntity(EntityWrapper entity) override;

    ~SetAction() override;
};

struct SequenceAction : public SetAction {
    static SequenceAction *create();

    bool step(double dt) override;
};

struct ParallelAction : public SetAction {
    static ParallelAction *create();

    bool step(double dt) override;
};

struct MoveTo : public Action {
    float targetX, targetY;
    float startX, startY;
    TransformComponent *entityTc;

    MoveTo(float targetX, float targetY, float pDuration = 0);

    static MoveTo *create(float x, float y, float duration = 0) {
        return new MoveTo(x, y, duration);
    };

    bool step(double tickMs) override;

    void begin() override;
};

struct MoveBy : public Action {
    float targetX, targetY;
    TransformComponent *entityTc;

    MoveBy(float targetX, float targetY, float pDuration = 0);

    static MoveBy *create(float x, float y, float duration) {
        return new MoveBy(x, y, duration);
    };

    bool step(double tickMs) override;

    void begin() override;
};

struct RotateTo : public Action {
    float targetDegree;
    float startDegree;
    TransformComponent *entityTc;

    RotateTo(float targetDegree, float pDuration, bool clockwise, std::function<void()> endCallback);

    static RotateTo *create(float pTargetDegree, float pDuration = 0,
                            bool clockwise = true,
                            std::function<void()> endCallback = nullptr) {
        return new RotateTo(pTargetDegree, pDuration, clockwise, endCallback);
    }

    bool step(double tickMs) override;

    void begin() override;
};

struct RotateBy : public Action {
    float targetDegree;
    float delta;
    TransformComponent *entityTc;

    RotateBy(float targetDegree, float pDuration, bool clockwise, std::function<void()> endCallback);

    static RotateBy *create(float pTargetDegree, float pDuration = 0,
                            bool clockwise = true,
                            std::function<void()> endCallback = nullptr) {
        return new RotateBy(pTargetDegree, pDuration, clockwise, endCallback);
    }

    bool step(double tickMs) override;

    void begin() override;
};


#endif //NORENGINE_ACTION_H
