#pragma once
#include <vector>
#include <functional>
#include "Entity.h"

class ObjectPool {
public:
    ObjectPool(size_t size) : nextAvailableID(2) {  // Initialize nextAvailableID as 2 (since player is 0 and base is 1)
        entities.reserve(size);  // Reserve memory for entities
        for (size_t i = 0; i < size; ++i) {
            entities.emplace_back(Entity(nextAvailableID++));  // Create entities
            entities[i].inUse = false;  // Initialize all entities as not in use
        }
    }

    // Get an inactive entity from the pool
    Entity* acquire() {
        for (auto& entity : entities) {
            if (!entity.inUse) {
                entity.inUse = true;  // Mark the entity as in use
                return &entity;
            }
        }
        return nullptr;
    }

    // Release entity to pool
    void release(Entity::ID entityId) {
        if (entityId >= 2 && entityId < nextAvailableID) {  // Ensure valid ID range
            entities[entityId - 2].inUse = false;
        }
    }

    // Iterate over active entities
    void forEachActive(std::function<void(Entity&)> func) {
        for (auto& entity : entities) {
            if (entity.inUse) {
                func(entity);
            }
        }
    }

    // Retrieve entities
    std::vector<Entity>& getEntities() {
        return entities;
    }

private:
    std::vector<Entity> entities;
    unsigned int nextAvailableID;
};
