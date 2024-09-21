    #pragma once
    #include <unordered_map>
    #include <typeindex>
    #include <vector>
    #include "Entity.h"
    #include "Components.h"

    class ComponentManager {
    public:
        template <typename T>
        void addComponent(Entity::ID entity, T component) {
            components[typeid(T)][entity] = new T(component);
            inUse[entity] = true;
        }

        template <typename T>
        void removeComponent(Entity::ID entity) {
            components[typeid(T)].erase(entity);
        }

        template <typename T>
        T* getComponent(Entity::ID entity) {
            auto it = components[typeid(T)].find(entity);
            if (it != components[typeid(T)].end()) {
                return static_cast<T*>(it->second);
            }
            return nullptr;
        }

        template <typename... Components>
        std::vector<Entity::ID> getEntitiesWithComponents() {
            std::vector<Entity::ID> result;

            if constexpr (sizeof...(Components) > 0) {
                // Retrieve entities with the first component or its derived types
                result = getEntitiesWithComponentHelper<typename std::tuple_element<0, std::tuple<Components...>>::type>();

                // Filter based on other components
                for (auto entity = result.begin(); entity != result.end();) {
                    if (!hasAllComponents<Components...>(*entity)) {
                        entity = result.erase(entity);
                    }
                    else {
                        ++entity;
                    }
                }
            }

            return result;
        }

        template <typename ComponentType>
        std::vector<Entity::ID> getEntitiesWithComponentHelper() {
            std::vector<Entity::ID> result;

            for (auto& componentPair : components) {
                for (auto& entityPair : componentPair.second) {
                    Entity::ID entityId = entityPair.first;
                    Component* componentPtr = entityPair.second;

                    // Check if the component is exactly the requested type or a derived type
                    if (dynamic_cast<ComponentType*>(componentPtr)) {
                        result.push_back(entityId);
                    }
                }
            }

            return result;
        }

        bool isEntityInUse(Entity::ID entity) const {
            auto it = inUse.find(entity);
            return (it != inUse.end()) ? it->second : false;
        }

        void setEntityInUse(Entity::ID entity, bool use) {
            inUse[entity] = use;
        }

        ~ComponentManager() {
            for (auto& pair : components) {
                for (auto& entityComp : pair.second) {
                    delete entityComp.second;
                }
            }
        }

    private:
        std::unordered_map<std::type_index, std::unordered_map<Entity::ID, Component*>> components;
        std::unordered_map<Entity::ID, bool> inUse;

        template <typename T>
        std::vector<Entity::ID> getEntitiesWithComponent() {
            std::vector<Entity::ID> entities;
            auto it = components.find(typeid(T));
            if (it != components.end()) {
                for (const auto& pair : it->second) {
                    entities.push_back(pair.first);
                }
            }
            return entities;
        }

        template <typename First, typename... Rest>
        bool hasAllComponents(Entity::ID entity) {
            if (!hasComponent<First>(entity)) {
                return false;
            }
            if constexpr (sizeof...(Rest) > 0) {
                return hasAllComponents<Rest...>(entity);
            }
            return true;
        }

        template <typename T>
        bool hasComponent(Entity::ID entity) {
            return components[typeid(T)].count(entity) > 0;
        }
    };
