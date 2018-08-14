#ifndef NORENGINE_ENTITYMANAGER_H
#define NORENGINE_ENTITYMANAGER_H


#include <unordered_map>
#include <set>
#include "Entity.h"
#include "Definitions.h"

constexpr uint16_t INITIAL_POOL_COUNT{16};
constexpr uint16_t INITIAL_INDEXES_COUNT{16};

struct EntityBuilder;

class EntityManager {
    EntityWrapper entityIndex = 0;
    EntityWrapper entitiesIndexPivot = 0;
    std::unordered_map<EntityWrapper, Entity *> entitiesMap;
    std::unordered_map<std::string, Entity *> entitiesByName;

    std::set<EntityWrapper> entitiesToDelete;

    EntityBuilder *entityBuilder;

    // cached components lists
    std::vector<SpriteComponent *> spriteComponents;
    bool spriteComponentsDirty = false;

public:

    std::vector<EntityWrapper> entitiesIndex;
    std::vector<Entity> entities;

    explicit EntityManager();

    virtual ~EntityManager();

    EntityBuilder &create(std::string name, glm::vec4 bounds, Tag tag);

    void clearEntities();

    void deleteEntity(EntityWrapper entity);

    int deleteEntity(std::string name);

    template<class T>
    T *addAndGetComponent(EntityWrapper entity, ComponentType type) {
        return reinterpret_cast<T *>(addComponent(entity, type));
    }

    Entity *getEntity(EntityWrapper index);

    Entity *getEntity(std::string name);

    Entity *getEntity(Tag tag);

    bool hasComponent(EntityWrapper entity, ComponentType type);

    void update(double deltaTime);

    template<class T>
    T *getComponent(EntityWrapper entity, ComponentType type) {
        return reinterpret_cast<T *>(entitiesMap[entity]->components[type]);
    }

    template<class T>
    T *getComponent(std::string entity, ComponentType type) {
        return reinterpret_cast<T *>(entitiesByName[entity]->components[type]);
    }

    template<class T>
    T *getComponent(Tag tag, ComponentType type) {
        for (int i = 0; i < entitiesIndexPivot; ++i) {
            Entity &entity = entities[entitiesIndex[i]];

            if (entity.tag == tag) {
                return reinterpret_cast<T *>(entity.components[type]);
            }
        }

        SDL_Log("Component not found Tag: %d Type: %d", tag, type);

        return nullptr;
    }

    std::vector<SpriteComponent *> getSpriteComponents();

    template<class T>
    std::vector<T *> getComponents(ComponentType type, Tag tag = Tag::ANY) {
        auto temp = std::vector<T *>();

        for (int i = 0; i < entitiesIndexPivot; ++i) {
            Entity &entity = entities[entitiesIndex[i]];

            if (tag != Tag::ANY && entity.tag != tag) continue;

            if (entity.componentBitset[type]) {
                temp.push_back(reinterpret_cast<T *>(entity.components[type]));
            }
        }

        return temp;
    }

    inline std::vector<Entity *> getEntities() {
        auto temp = std::vector<Entity *>();

        for (int i = 0; i < entitiesIndexPivot; ++i) {
            Entity &entity = entities[entitiesIndex[i]];
            temp.push_back(&entity);
        }

        return temp;
    }

    void addAction(EntityWrapper action, Action *pAction);

    Component *addComponent(EntityWrapper entity, ComponentType type);

private:
    void growEntities();

    void processDeletions();

    void printEntitiesInfo();
};

struct EntityBuilder {
    Entity *entity;
    EntityManager *entityManager;

    EntityBuilder(EntityManager *pManager);

    EntityBuilder &component(ComponentType componentType);

    EntityBuilder &transform(glm::vec2 origin, float rotation);

    EntityBuilder &animation(std::vector<std::string> textures, int interval, glm::vec2 anchor, ZOrder z);

    EntityBuilder &texture(std::string texture, glm::vec2 anchor, ZOrder zOrder);

    EntityBuilder &texture(std::string texture, glm::vec2 anchor);

    EntityBuilder &texture(std::string texture, ZOrder zOrder);

    EntityBuilder &texture(std::string texture);

    EntityBuilder &bodyOffset(glm::vec4 offset);

    EntityBuilder &inputOffset(glm::vec4 offset);

    EntityBuilder &motion(glm::vec2 motion, float velocity, bool targeted = true);

    EntityBuilder &text(std::string text);

    EntityBuilder &text(std::string text, SDL_Color color);

    EntityBuilder &text(std::string text, glm::vec2 scale);

    EntityBuilder &text(std::string text, SDL_Color color, glm::vec2 scale);

    EntityBuilder &enemy(EnemyType type);

    EntityWrapper get();
};


#endif //NORENGINE_ENTITYMANAGER_H
