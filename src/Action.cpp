#include <Game.h>
#include "Action.h"
#include "EntityManager.h"

void Action::addToEntity(EntityWrapper entity) {
    target = entity;

    Game::entityManager->addAction(entity, this);
}

Action::Action() {
    target = -1;
    endCallback = nullptr;
    isDone = false;
    duration = 0;
    elapsedTime = 0;
    speed = 1;
    lastPercent = 0;
}

void Action::setSpeed(float s) {
    speed = s;
}

bool Action::step(double tickMs) {
    if (!began) {
        begin();
        began = true;
    }

    elapsedTime += tickMs * speed;

    if (duration == 0 || elapsedTime >= duration) {
        percent = 1;
        isDone = true;
        end();
    } else {
        percent = elapsedTime / duration;
    }

    percent = interpolation(percent);

    return isDone;
}

Action::~Action() {
//    printf("Action dest: %f\n", duration);
}

void Action::begin() {

}

void Action::end() {
    if (endCallback != nullptr) endCallback();
}

SetAction *SetAction::add(Action *action) {
    actions.push_back(action);
    duration += action->duration;
    return this;
}

void SetAction::addToEntity(EntityWrapper entity) {
    Action::addToEntity(entity);

    for (auto a : actions)
        a->target = target;
}

SetAction::~SetAction() {
//    printf("SetAction dest: %f\n", duration);
    for (auto a : actions) delete a;
}

SequenceAction *SequenceAction::create() {
    return new SequenceAction();
}

bool SequenceAction::step(double dt) {
    if (actions.size() == 0) {
        isDone = true;
        return true;
    }

    actions[0]->step(dt);

    if (actions[0]->isDone) actions.erase(actions.begin());

    return isDone;
}

ParallelAction *ParallelAction::create() {
    return new ParallelAction();
}

bool ParallelAction::step(double dt) {
    if (actions.size() == 0) {
        isDone = true;
        return true;
    }

    auto tempVec = actions;
    for (int i = 0; i < tempVec.size(); ++i) {
        tempVec[i]->step(dt);

        if (tempVec[i]->isDone) actions.erase(actions.begin() + i);
    }

    return isDone;
}

MoveTo::MoveTo(float targetX, float targetY, float pDuration) :
        targetX(targetX), targetY(targetY) {
    duration = pDuration;
}

bool MoveTo::step(double tickMs) {
    Action::step(tickMs);

    entityTc->setXY((float) (startX + ((targetX - startX) * percent)),
                    (float) (startY + ((targetY - startY) * percent)));

    return isDone;
}

void MoveTo::begin() {
    entityTc = EM->getComponent<TransformComponent>(target, ComponentType::TRANSFORM);
    startX = entityTc->rect.x;
    startY = entityTc->rect.y;
}

MoveBy::MoveBy(float targetX, float targetY, float pDuration) :
        targetX(targetX), targetY(targetY) {
    duration = pDuration;
}

bool MoveBy::step(double tickMs) {
    Action::step(tickMs);

    entityTc->translate((float) (targetX * (percent - lastPercent)),
                        (float) (targetY * (percent - lastPercent)));

    lastPercent = percent;
    return isDone;
}

void MoveBy::begin() {
    entityTc = EM->getComponent<TransformComponent>(target, ComponentType::TRANSFORM);
}

RotateTo::RotateTo(float targetDegree, float pDuration,
                   bool pClockwise,
                   std::function<void()> pEndCallback) : targetDegree(targetDegree) {
    duration = pDuration;
    clockwise = pClockwise;
    endCallback = pEndCallback;
}

bool RotateTo::step(double tickMs) {
    Action::step(tickMs);

    if (clockwise) {
        entityTc->setRotation((float) (startDegree + (targetDegree - startDegree) * percent));
    } else {
        entityTc->setRotation((float) (startDegree - (targetDegree - startDegree) * percent));
    }

    return isDone;
}

void RotateTo::begin() {
    entityTc = EM->getComponent<TransformComponent>(target, ComponentType::TRANSFORM);
    startDegree = entityTc->rotation;
}

RotateBy::RotateBy(float targetDegree, float pDuration,
                   bool pClockwise,
                   std::function<void()> pEndCallback) : targetDegree(targetDegree) {
    duration = pDuration;
    clockwise = pClockwise;
    endCallback = pEndCallback;
}

bool RotateBy::step(double tickMs) {
    Action::step(tickMs);

    if (clockwise) {
        entityTc->setRotation(entityTc->rotation + (float) (targetDegree * (percent - lastPercent)));
    } else {
        entityTc->setRotation(entityTc->rotation - (float) (targetDegree * (percent - lastPercent)));
    }

    lastPercent = percent;
    return isDone;
}

void RotateBy::begin() {
    entityTc = EM->getComponent<TransformComponent>(target, ComponentType::TRANSFORM);
//    delta = (float) std::fmod(360 + targetDegree - entityTc->rotation, 360);
}
