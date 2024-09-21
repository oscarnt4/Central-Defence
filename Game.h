#include "Systems.h"
#include "ObjectPool.h"
#include "Debug.h"

class Game {
public:
    Game();
    void run();

    ComponentManager& getComponentManager() { return componentManager; }
private:
    void gameLoop(sf::Clock& clock, float timePerFrame);
    void processInput();
    void update(float deltaTime);
    void render(); 

    void initialisePlayer();
    void initialiseBase();
    void initialiseProjectile(float startX, float startY, float velocityX, float velocityY);

    void updateBaseColour();

    sf::RenderWindow mWindow;
    ComponentManager componentManager;
    MovementSystem movementSystem;
    RotationSystem rotationSystem;
    RenderSystem renderSystem;
    CollisionSystem collisionSystem;
    HealthSystem healthSystem;
    ProjectileSpawnSystem projectileSpawnSystem;
    GameManager gameManager;
    Debug debug;

    Entity::ID playerEntity;
    Entity::ID baseEntity;

    ObjectPool projectilePool;
};
