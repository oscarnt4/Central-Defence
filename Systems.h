#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ComponentManager.h"
#include "ObjectPool.h"
#include <SFML/Graphics.hpp>
#include <random>

class MovementSystem {
public:
    void update(ComponentManager& manager, float deltaTime);
};

class RotationSystem {
public:
    void update(ComponentManager& manager, float deltaTime);
};

class RenderSystem {
public:
    void render(ComponentManager& manager, sf::RenderWindow& window);
};

class GameManager;

class CollisionSystem {
public:
    CollisionSystem(ObjectPool& projectilePool, GameManager& gameManager)
        : projectilePool(projectilePool),  gameManager(gameManager) {}

    void update(ComponentManager& manager, sf::RenderWindow& window);
    void scalePlayerCollider(ComponentManager& manager);

private:
    ObjectPool& projectilePool;
    GameManager& gameManager;

    // General collision detection function
    template <typename ColliderType1, typename ColliderType2>
    void checkAllCollisions(ComponentManager& manager, sf::RenderWindow& window, const std::vector<Entity::ID>& entities1, const std::vector<Entity::ID>& entities2);

    // Specific collision detection functions
    bool checkCollision(BoxCollider* box1, BoxCollider* box2);
    bool checkCollision(BoxCollider* box, CircleCollider* circle);
    bool checkCollision(CircleCollider* circle1, CircleCollider* circle2);

    // Handle the collision between two entities
    void handleCollision(ComponentManager& manager, sf::RenderWindow& window, Entity::ID entity1, Entity::ID entity2);

    // Function to check collision between a box and a circle
    bool checkBoxCircleCollision(BoxCollider* box, CircleCollider* circle);

    void scalePlayerRotation(ComponentManager& manager, sf::RenderWindow& window);

    // Utility function for clamping values
    template <typename T>
    static T clamp(const T& value, const T& min, const T& max) {
        return (value < min) ? min : (value > max) ? max : value;
    }
};

class ProjectileSpawnSystem {
public:
    ProjectileSpawnSystem(ObjectPool& projectilePool, float initialTimeWindow)
        : projectilePool(projectilePool), timeWindow(initialTimeWindow), initialTimeWindow(initialTimeWindow),
        elapsedTime(0.f), level(1), totalProjectiles(10), projectilesRemaining(10),
        gen(rd())
    {
        levelTime = totalProjectiles * timeWindow;
    }

    void update(ComponentManager& manager, sf::RenderWindow& window, float deltaTime);
    void reset(ComponentManager& manager);

    int totalSpawned = 0;

private:
    ObjectPool& projectilePool;
    float timeWindow;
    float initialTimeWindow;
    float elapsedTime;
    int level;
    int totalProjectiles;
    int projectilesRemaining;
    float levelTime;
    bool powerUpSpawned = false;

    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen;  // Declare the generator without initializing it

    void spawnPowerUp(ComponentManager& manager, sf::RenderWindow& window);
    void spawnProjectile(ComponentManager& manager, sf::RenderWindow& window);
    void nextLevel();
    void launchProjectile(ComponentManager& manager, sf::RenderWindow& window);
    void launchSpeedPowerUp(ComponentManager& manager, sf::RenderWindow& window);
    void launchSizePowerUp(ComponentManager& manager, sf::RenderWindow& window);
    sf::Vector2f getRandomEdgePosition(sf::RenderWindow& window);
};

class HealthSystem {
public:
    HealthSystem(ObjectPool& projectilePool, GameManager& gameManager)
        : projectilePool(projectilePool), gameManager(gameManager) {}

    void update(ComponentManager& manager, sf::RenderWindow& window);
    void applyDamage(ComponentManager& manager, Entity::ID entity, int damage);

private:
    ObjectPool& projectilePool;
    GameManager& gameManager;

    void checkBaseHealth(ComponentManager& manager);
};

class GameManager {
public:
    GameManager(ProjectileSpawnSystem& projectileSystem, CollisionSystem& collisionSystem)
        : projectileSpawnSystem(projectileSystem), collisionSystem(collisionSystem) {}  // Pass reference to systems that need to be reset

    void resetGame(ComponentManager& manager, sf::RenderWindow& window);
    void onBaseHealthDepleted(ComponentManager& manager, sf::RenderWindow& window);

private:
    ProjectileSpawnSystem& projectileSpawnSystem;
    CollisionSystem& collisionSystem;
};

#endif 