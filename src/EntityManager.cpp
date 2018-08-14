#include <algorithm>
#include "EntityManager.h"
#include "SignalManager.h"

EntityManager::EntityManager() {
    entities.reserve(INITIAL_POOL_COUNT);
    entitiesIndex.reserve(INITIAL_INDEXES_COUNT);

    for (int32_t i = 0; i < INITIAL_INDEXES_COUNT; ++i) {
        entitiesIndex.push_back(i);
    }

    entityBuilder = new EntityBuilder(this);
}

void EntityManager::update(double deltaTime) {
    // update entities
    for (int i = 0; i < entitiesIndexPivot; ++i) {
        Entity &entity = entities[entitiesIndex[i]];

        if (entity.action != nullptr && entity.action->step(deltaTime))
            entity.removeAction();

        // update projectiles
        if (entity.tag == Tag::ENEMY_TAG) {
            auto projectileBodies = getComponents<BodyComponent>(ComponentType::BODY, Tag::PROJECTILE);
            auto playerBodies = getComponents<BodyComponent>(ComponentType::BODY, Tag::PLAYER);
            auto tc = entity.getComponent<TransformComponent>(ComponentType::TRANSFORM);
            auto body = entity.getComponent<BodyComponent>(ComponentType::BODY);

            for (int j = 0; j < projectileBodies.size(); ++j) {
                auto projectileTc = getComponent<TransformComponent>(projectileBodies[j]->owner, TRANSFORM);
                auto enemyBounds = body->getBounds(tc);
                auto projBounds = projectileBodies[j]->getBounds(projectileTc);

                auto intersect =
                        Util::isIntersectRect(enemyBounds.x, enemyBounds.y, enemyBounds.z, enemyBounds.w,
                                              projBounds.x, projBounds.y, projBounds.z, projBounds.w);

                if (intersect) {
                    SignalManager::fire(Signal::INCREASE_SCORE);
                    SignalManager::fireGlobal(Signal::PROJECTILE_HIT, SignalArgs(projectileTc->owner, entity.id));
                }
            }

            for (int k = 0; k < playerBodies.size(); ++k) {
                auto playerTc = getComponent<TransformComponent>(playerBodies[k]->owner, ComponentType::TRANSFORM);
                auto enemyBody = body->getBounds(tc);
                auto playerBody = playerBodies[k]->getBounds(playerTc);
                auto enemyBounds = body->getBounds(tc);
                auto playerBounds = playerBodies[k]->getBounds(playerTc);

                auto intersect = Util::isIntersectRect(enemyBounds.x, enemyBounds.y, enemyBounds.z, enemyBounds.w,
                                                       playerBounds.x, playerBounds.y, playerBounds.z, playerBounds.w);

                if (intersect) {
                    SignalManager::fireGlobal(Signal::PLAYER_HIT, SignalArgs(entity.id, playerTc->owner));
                }
            }
        }
    }

    auto motions = getComponents<MotionComponent>(ComponentType::MOTION);

    for (int j = 0; j < motions.size(); ++j) {
        auto tc = getComponent<TransformComponent>(motions[j]->owner, ComponentType::TRANSFORM);

        if (motions[j]->targeted) {
            motions[j]->direction = Util::getBearing(
                    tc->originX(), tc->originY(),
                    motions[j]->targetX, motions[j]->targetY);
            tc->setRotation(motions[j]->direction);
        }

        tc->translate((float) (motions[j]->velocity * deltaTime * std::sin(motions[j]->direction / 180 * M_PI)),
                      -(float) (motions[j]->velocity * deltaTime * std::cos(motions[j]->direction / 180 * M_PI)));

//        std::cout << "Rotation: " << tc->rotation << " X: " << tc->rect.x << " Y: " << tc->rect.y << std::endl;

        // boundary check for entities with MOTION component
        if (tc->rect.x >= Game::instance->virtualWidth + 20 || tc->rect.x <= -20 ||
            tc->rect.y >= Game::instance->virtualHeight + 20 || tc->rect.y <= -20) {
            deleteEntity(motions[j]->owner);
        }
    }

    // after update stuff
    processDeletions();
}

EntityBuilder &EntityManager::create(std::string name, glm::vec4 bounds, Tag tag) {
    bool grow = false;
    if (entities.capacity() == entities.size()) {
        grow = true;
    }

    if (entities.size() == entitiesIndexPivot) {
        entities.emplace_back();
        if (grow) growEntities();
    }

    auto entity = &entities[entitiesIndex[entitiesIndexPivot]];
    entity->init(name, entityIndex++, tag);
    entitiesMap[entity->id] = entity;
    entitiesByName[entity->name] = entity;

    auto tc = addAndGetComponent<TransformComponent>(entity->id, ComponentType::TRANSFORM);
    tc->init(bounds.x, bounds.y, bounds.z, bounds.w);

    entitiesIndexPivot++;

    entityBuilder->entity = entity;

//    std::cout << "CREATED Name: " << name << " Pivot: " << entitiesIndexPivot << " Index Size: " << entitiesIndex.size()
//              << " Entities Size: " << entities.size() << std::endl;

    return *entityBuilder;
}

void EntityManager::deleteEntity(EntityWrapper entity) {
    entitiesToDelete.insert(entity);
}

int EntityManager::deleteEntity(std::string name) {
    printf("%f\tEntities to delete: %s\n", Util::getTimeStamp(), name.c_str());

    int i = 0;
    for (auto itr = entities.begin(); itr != entities.end(); ++itr) {
        if (itr->name == name) {
            deleteEntity(itr->id);
            i++;
        }
    }

    return i;
}

bool EntityManager::hasComponent(EntityWrapper entity, ComponentType type) {
    return entitiesMap[entity]->componentBitset[(uint16_t) type];
}

Component *EntityManager::addComponent(EntityWrapper entity, ComponentType type) {
    if (entitiesMap[entity]->componentBitset[type]) {
        std::cout << "Already added component: " << entitiesMap[entity]->name << std::endl;

        return entitiesMap[entity]->components[type];
    }

    entitiesMap[entity]->componentBitset.set(type);
    Component *component = nullptr;

    switch (type) {
        case ComponentType::TRANSFORM:
            component = new TransformComponent();
            break;
        case ComponentType::ENEMY:
            component = new EnemyComponent();
            break;
        case ComponentType::MOTION:
            component = new MotionComponent();
            break;
        case ComponentType::SPRITE:
            spriteComponentsDirty = true;
            component = new SpriteComponent();
            break;
        case ComponentType::INPUT_C:
            component = new InputComponent();
            break;
        case ComponentType::TEXT:
            component = new TextComponent();
            break;
        case ComponentType::BODY:
            component = new BodyComponent();
            break;
        default:
            component = new Component();
            std::cout << "Component TYPE was WRONG!" << std::endl;
            break;
    }

    entitiesMap[entity]->components[type] = component;
    component->isAlive = true;
    component->owner = entitiesMap[entity]->id;
    component->ownerName = std::string(entitiesMap[entity]->name) + Util::toString(entitiesMap[entity]->id);
//    printf("Added comp: %d to: %s - %p\n", component->type, component->ownerName.c_str(), (void *) &*component);

    return component;
}

Entity *EntityManager::getEntity(EntityWrapper id) {
    return entitiesMap[id];
}

void EntityManager::clearEntities() {
    for (int i = 0; i < entities.size(); ++i) {
        for (int j = 0; j < entities[i].components.size(); ++j) {
            if (entities[i].components[j] != nullptr)
                delete entities[i].components[j];
        }
    }

    entities.erase(entities.begin(), entities.end());
    entities.reserve(INITIAL_POOL_COUNT);
    entitiesMap.clear();
    entitiesByName.clear();
    entitiesToDelete.clear();
    entitiesIndex.clear();

    for (int k = 0; k < entitiesIndex.capacity(); ++k) {
        entitiesIndex.push_back(k);
    }

    entityIndex = 0;
    entitiesIndexPivot = 0;

    SDL_Log("Entities cleared! Cap: %d/%d Indexes Cap: %d/%d Pivot: %d\n",
            entities.capacity(), entities.size(), entitiesIndex.capacity(), entitiesIndex.size(), entitiesIndexPivot);
}

Entity *EntityManager::getEntity(Tag tag) {
    for (int i = 0; i < entitiesIndexPivot; ++i) {
        if (entities[entitiesIndex[i]].tag == tag) {
            return &entities[entitiesIndex[i]];
        }
    }

    return nullptr;
}

void EntityManager::addAction(EntityWrapper entity, Action *pAction) {
    auto ent = getEntity(entity);

    if (ent->action != nullptr) {
        ent->action->add(pAction);
    } else {
        auto sa = new SequenceAction();
        ent->action.reset((SequenceAction *) sa->add(pAction));
    }
}

void EntityManager::growEntities() {
    int size = entitiesIndex.size();
    for (int i = size; i < size * 2; ++i) {
        entitiesIndex.push_back(i);
    }

    SDL_Log("Grow: %d/%d", entitiesIndex.capacity(), entitiesIndex.size());

    entitiesMap.clear();
    entitiesByName.clear();

    for (int j = 0; j < entitiesIndexPivot; ++j) {
        entitiesMap[entities[entitiesIndex[j]].id] = &entities[entitiesIndex[j]];
        entitiesByName[entities[entitiesIndex[j]].name] = &entities[entitiesIndex[j]];
    }
}

EntityManager::~EntityManager() {
    delete entityBuilder;
}

void EntityManager::processDeletions() {
    for (auto entity : entitiesToDelete) {
        auto entityDlt = entitiesMap[entity];

//
//    std::cout << Util::getTimeStamp() << "\t" << "Ent. Deleted: " << entityDlt->name << " " << entityDlt->id <<
//              " - " << &*entityDlt << std::endl;

        for (int i = 0; i < entityDlt->components.size(); ++i) {
            if (entityDlt->components[i] != nullptr) {
                // mark components that cached as dirty
                if (i == ComponentType::SPRITE) {
                    spriteComponentsDirty = true;
                }

                entityDlt->components[i]->isAlive = false;
                delete entityDlt->components[i];
                entityDlt->components[i] = nullptr;
            }
        }

        for (int j = 0; j < entitiesIndexPivot; ++j) {
            if (entities[entitiesIndex[j]].id == entity) {
                entitiesIndexPivot--;
                std::swap(entitiesIndex[j], entitiesIndex[entitiesIndexPivot]);
                break;
            }
        }
//
//    std::cout << Util::getTimeStamp()
//              << "DELETED Name: " << entityDlt->name << " Pivot: " << entitiesIndexPivot << " Index Size: "
//              << entitiesIndex.size() << " Entities Size: "
//              << entities.size() << std::endl;

        for (auto itr : entityDlt->children) {
            deleteEntity(itr->id);
        }

        entitiesMap.erase(entityDlt->id);
        entityDlt->reset();
    }

    entitiesToDelete.clear();
}

Entity *EntityManager::getEntity(std::string name) {
    if (entitiesByName.count(name) == 0) {
        std::cout << "Entity is not in the map: " << name << std::endl;
    }

    return entitiesByName[name];
}

std::vector<SpriteComponent *> EntityManager::getSpriteComponents() {
    if (!spriteComponentsDirty) {
        return spriteComponents;
    } else {
        spriteComponentsDirty = false;
        spriteComponents.clear();
    }

    for (int i = 0; i < entitiesIndexPivot; ++i) {
        Entity &entity = entities[entitiesIndex[i]];

        if (entity.componentBitset[SPRITE]) {
            spriteComponents.push_back((SpriteComponent *) entity.components[SPRITE]);
        }
    }

    std::sort(spriteComponents.begin(), spriteComponents.end(),
              [](const SpriteComponent *first, const SpriteComponent *second) {
                  return second->zOrder > first->zOrder;
              });

    return spriteComponents;
}

void EntityManager::printEntitiesInfo() {
    SDL_Log("=========sSSSsSS========\n");
    for (int i = 0; i < entitiesIndexPivot; ++i) {
        Entity &entity = entities[entitiesIndex[i]];

        SDL_Log("------------\nEnt Name: %s/%d, IsAlive: %d Pivot: %d\n", entity.name.c_str(), entity.id,
                entity.isAlive, entitiesIndexPivot);

        for (int j = 0; j < entity.components.size(); ++j) {
            if (entity.components[j] == nullptr) continue;

            SDL_Log("\tComponent: %d, IsAlive: %d\n", entity.components[j]->type, entity.components[j]->isAlive);
        }
    }

    SDL_Log("=========sSSSsSS========\n");
}

EntityBuilder &EntityBuilder::component(ComponentType componentType) {
    entityManager->addComponent(entity->id, componentType);

    return *this;
}

EntityBuilder &EntityBuilder::animation(std::vector<std::string> t, int i, glm::vec2 anchor, ZOrder z) {
    auto sprite = entityManager->addAndGetComponent<SpriteComponent>(entity->id, ComponentType::SPRITE);
    auto tc = entityManager->getComponent<TransformComponent>(entity->id, ComponentType::TRANSFORM);

    auto animations = new AnimationComponent();
    animations->interval = i;

    for (int j = 0; j < t.size(); ++j) {
        animations->animation.push_back(AM->getAsset(t[j]));
    }

    sprite->initAnimation(animations, anchor, tc, z);

    return *this;
}

EntityBuilder &EntityBuilder::texture(std::string texture, glm::vec2 anchor, ZOrder zOrder) {
    auto sprite = entityManager->addAndGetComponent<SpriteComponent>(entity->id, ComponentType::SPRITE);
    auto tc = entityManager->getComponent<TransformComponent>(entity->id, ComponentType::TRANSFORM);
    sprite->initSprite(AM->getAsset(texture), anchor, tc, zOrder);

    return *this;
}

EntityBuilder &EntityBuilder::texture(std::string pTexture, ZOrder zOrder) {
    return texture(pTexture, {0.5f, 0.5f}, zOrder);
}

EntityBuilder &EntityBuilder::texture(std::string pTexture, glm::vec2 anchor) {
    return texture(pTexture, anchor, ZOrder::DEFAULT);
}

EntityBuilder &EntityBuilder::texture(std::string pTexture) {
    return texture(pTexture, {0.5f, 0.5f}, ZOrder::DEFAULT);
}

EntityBuilder &EntityBuilder::bodyOffset(glm::vec4 offset) {
    if (!entityManager->hasComponent(entity->id, BODY)) SDL_Log("bodyOffset without BODY component!");

    entityManager->getComponent<BodyComponent>(entity->id, ComponentType::BODY)->init(offset);

    return *this;
}

EntityBuilder &EntityBuilder::inputOffset(glm::vec4 offset) {
    if (!entityManager->hasComponent(entity->id, INPUT_C)) SDL_Log("inputOffset without INPUT_C component!");

    entityManager->getComponent<InputComponent>(entity->id, ComponentType::INPUT_C)->bounds = offset;

    return *this;
}

EntityBuilder &EntityBuilder::motion(glm::vec2 motion, float velocity, bool targeted) {
    auto comp = entityManager->addAndGetComponent<MotionComponent>(entity->id, ComponentType::MOTION);
    auto tc = entityManager->getComponent<TransformComponent>(entity->id, ComponentType::TRANSFORM);
    comp->targeted = targeted;
    comp->targetX = motion.x;
    comp->targetY = motion.y;
    comp->velocity = velocity;
    comp->direction = tc->rotation;

    return *this;
}

EntityBuilder &EntityBuilder::text(std::string pText) {
    return text(pText, {255, 255, 255, 255}, {1.0f, 1.0f});
}

EntityBuilder &EntityBuilder::text(std::string pText, SDL_Color color) {
    return text(pText, color, {1.0f, 1.0f});
}

EntityBuilder &EntityBuilder::text(std::string pText, glm::vec2 scale) {
    return text(pText, {255, 255, 255, 255}, scale);
}

EntityBuilder &EntityBuilder::text(std::string text, SDL_Color color, glm::vec2 scale) {
    entityManager->addAndGetComponent<TextComponent>(entity->id, ComponentType::TEXT)
            ->init(text, color, Align::CENTER, scale);

    return *this;
}

EntityBuilder &EntityBuilder::enemy(EnemyType type) {
    auto enemyComponent = entityManager->getComponent<EnemyComponent>(entity->id, ComponentType::ENEMY);
    enemyComponent->type = type;
    Game::enemyManager->initEnemy(enemyComponent, (MotionComponent *) entity->components[MOTION]);

    return *this;
}

EntityWrapper EntityBuilder::get() {
    return entity->id;
}

EntityBuilder::EntityBuilder(EntityManager *pManager) {
    entityManager = pManager;
}

EntityBuilder &EntityBuilder::transform(glm::vec2 origin, float rotation) {
    auto tc = entityManager->getComponent<TransformComponent>(entity->id, ComponentType::TRANSFORM);
    tc->setXY(origin.x - tc->rect.w / 2, origin.y + tc->rect.h / 2);
    tc->setRotation(rotation);

    return *this;
}
