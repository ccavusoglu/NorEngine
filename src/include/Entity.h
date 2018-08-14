#ifndef NORENGINE_ENTITY_H
#define NORENGINE_ENTITY_H


#include "Glm.h"
#include "Component.h"
#include "Action.h"
#include <cstdio>
#include <SDL_surface.h>
#include <vector>
#include <bitset>
#include <array>
#include <iostream>
#include <memory>

enum Tag {
    ANY = 0,
    ENEMY_TAG,
    PLAYER,
    PROJECTILE,
    UI_HP
};

constexpr int MAX_COMPONENTS{16};

struct Entity {
    EntityWrapper id;
    std::string name;
    Tag tag;
    bool isAlive;

    Entity *parent;
    std::vector<Entity *> children;

    std::bitset<MAX_COMPONENTS> componentBitset;
    std::array<Component *, MAX_COMPONENTS> components;
    std::unique_ptr<SequenceAction> action;

    void init(std::string pName, EntityWrapper pId, Tag pTag) {
        isAlive = true;
        name = pName;
        id = pId;
        tag = pTag;
    }

    void reset() {
        isAlive = false;
        action.release();
        parent = nullptr;

        componentBitset.reset();
    }

    void removeAction() {
        action.reset();
    }

    template<class T>
    T *getComponent(ComponentType type) const {
        auto ptr(components[(uint16_t) type]);
        return reinterpret_cast<T *>(ptr);
    }

    Entity() = default;

    Entity &operator=(const Entity &entity) = default;

    Entity(const Entity &entity) = default;

    Entity &operator=(Entity &&entity) = default;

//    Entity &operator=(Entity &&entity) {
//        name = entity.name;
//        id = entity.id;
//        tag = entity.tag;
//        isAlive = entity.isAlive;
//        parent = entity.parent;
//        children = entity.children;
//        componentBitset = entity.componentBitset;
//        components = entity.components;
//        action = std::move(entity.action);
//        onUpdate = entity.onUpdate;
//    }

    Entity(Entity &&entity) = default;

    ~Entity() = default;

    void print() {
        std::cout << "Entity: " << name << " Id: " << id << " CompSet: " << componentBitset << " - " << &*this
                  << std::endl;

        return;
        for (int i = 0; i < components.size(); ++i) {
            if (components[i] != nullptr) {
                std::cout << "\tComponent: " << components[i]->type << " Addr: " << &(*components[i]) << std::endl;
            }
        }
    };
};


#endif //NORENGINE_ENTITY_H
