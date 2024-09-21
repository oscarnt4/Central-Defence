#include "Systems.h"
#include "Commands.h"
#include <cmath> 
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void MovementSystem::update(ComponentManager& manager, float deltaTime) {
	auto entities = manager.getEntitiesWithComponents<Transform, Velocity>();
	for (auto entity : entities) {
		if (!manager.isEntityInUse(entity)) continue;

		Transform* transform = manager.getComponent<Transform>(entity);
		Velocity* velocity = manager.getComponent<Velocity>(entity);
		transform->x += velocity->dx * deltaTime;
		transform->y += velocity->dy * deltaTime;
	}
}

void RotationSystem::update(ComponentManager& manager, float deltaTime) {
	auto entities = manager.getEntitiesWithComponents<Rotation, Transform>();

	for (auto entity : entities) {
		if (!manager.isEntityInUse(entity)) continue;

		Rotation* rotation = manager.getComponent<Rotation>(entity);
		Transform* transform = manager.getComponent<Transform>(entity);

		if (rotation && transform) {
			// Calculate the angle increment
			float angleIncrement = rotation->speed * deltaTime;

			if (!rotation->clockwise) {
				angleIncrement = -angleIncrement;
			}

			// Update the angle
			rotation->angle += angleIncrement;

			// Keep the angle within the range [0, 360)
			if (rotation->angle >= 360.f) rotation->angle -= 360.f;
			if (rotation->angle < 0.f) rotation->angle += 360.f;

			// Calculate the new position based on the angle and radius
			transform->x = rotation->centerX + rotation->radius * std::cos(rotation->angle * M_PI / 180.f);
			transform->y = rotation->centerY + rotation->radius * std::sin(rotation->angle * M_PI / 180.f);
		}
	}
}



void RenderSystem::render(ComponentManager& manager, sf::RenderWindow& window) {
	auto entities = manager.getEntitiesWithComponents<Renderable, Transform>();

	for (auto entity : entities) {
		if (!manager.isEntityInUse(entity)) continue;

		Transform* transform = manager.getComponent<Transform>(entity);
		Renderable* renderable = manager.getComponent<Renderable>(entity);

		if (renderable->shape) {
			renderable->shape->setPosition(transform->x, transform->y);
			renderable->shape->setRotation(transform->angle);
			window.draw(*renderable->shape);
		}
		else if (renderable->sprite) {
			renderable->sprite->setPosition(transform->x, transform->y);
			renderable->sprite->setRotation(transform->angle);
			window.draw(*renderable->sprite);
		}
	}
}

void CollisionSystem::update(ComponentManager& manager, sf::RenderWindow& window) {
	// Get entities with either BoxCollider or CircleCollider and Transform components
	auto entitiesWithBoxColliders = manager.getEntitiesWithComponents<BoxCollider, Transform>();
	auto entitiesWithCircleColliders = manager.getEntitiesWithComponents<CircleCollider, Transform>();

	// Update positions for entities with BoxColliders
	for (auto entity : entitiesWithBoxColliders) {
		if (!manager.isEntityInUse(entity)) continue;  // Skip inactive entities

		Transform* transform = manager.getComponent<Transform>(entity);
		BoxCollider* boxCollider = manager.getComponent<BoxCollider>(entity);
		if (transform && boxCollider) {
			boxCollider->bounds.left = transform->x;
			boxCollider->bounds.top = transform->y;
		}
	}

	// Update positions for entities with CircleColliders
	for (auto entity : entitiesWithCircleColliders) {
		if (!manager.isEntityInUse(entity)) continue;  // Skip inactive entities

		Transform* transform = manager.getComponent<Transform>(entity);
		CircleCollider* circleCollider = manager.getComponent<CircleCollider>(entity);
		if (transform && circleCollider) {
			circleCollider->center.x = transform->x + circleCollider->radius;
			circleCollider->center.y = transform->y + circleCollider->radius;
		}
	}

	// Check for collisions between entities
	checkAllCollisions<BoxCollider, BoxCollider>(manager, window, entitiesWithBoxColliders, entitiesWithBoxColliders);
	checkAllCollisions<BoxCollider, CircleCollider>(manager, window, entitiesWithBoxColliders, entitiesWithCircleColliders);
	checkAllCollisions<CircleCollider, CircleCollider>(manager, window, entitiesWithCircleColliders, entitiesWithCircleColliders);
}

template <typename ColliderType1, typename ColliderType2>
void CollisionSystem::checkAllCollisions(ComponentManager& manager, sf::RenderWindow& window, const std::vector<Entity::ID>& entities1, const std::vector<Entity::ID>& entities2) {
	for (auto entity1 : entities1) {
		if (!manager.isEntityInUse(entity1)) continue;  // Skip inactive entities

		ColliderType1* collider1 = manager.getComponent<ColliderType1>(entity1);
		if (!collider1) continue;  // Ensure collider exists

		for (auto entity2 : entities2) {
			if (entity1 == entity2 || !manager.isEntityInUse(entity2)) continue;  // Skip itself and inactive entities

			ColliderType2* collider2 = manager.getComponent<ColliderType2>(entity2);
			if (collider2 && checkCollision(collider1, collider2)) {
				// Collision detected
				handleCollision(manager, window, entity1, entity2);  // Handle the collision between entity1 and entity2
				break;  // Exit the inner loop after handling collision
			}
		}
	}
}

bool CollisionSystem::checkCollision(BoxCollider* box1, BoxCollider* box2) {
	return box1->bounds.intersects(box2->bounds);
}

bool CollisionSystem::checkCollision(BoxCollider* box, CircleCollider* circle) {
	return checkBoxCircleCollision(box, circle);
}

bool CollisionSystem::checkCollision(CircleCollider* circle1, CircleCollider* circle2) {
	float dx = circle1->center.x - circle2->center.x;
	float dy = circle1->center.y - circle2->center.y;
	float distanceSquared = dx * dx + dy * dy;
	float radiusSum = circle1->radius + circle2->radius;
	return distanceSquared <= radiusSum * radiusSum;
}

bool CollisionSystem::checkBoxCircleCollision(BoxCollider* box, CircleCollider* circle) {
	// Find the closest point on the rectangle to the circle center
	float closestX = clamp(circle->center.x, box->bounds.left, box->bounds.left + box->bounds.width);
	float closestY = clamp(circle->center.y, box->bounds.top, box->bounds.top + box->bounds.height);

	// Calculate the distance from the closest point to the circle center
	float dx = closestX - circle->center.x;
	float dy = closestY - circle->center.y;

	// If the distance is less than the circle's radius, there is a collision
	return (dx * dx + dy * dy) <= (circle->radius * circle->radius);
}

void CollisionSystem::handleCollision(ComponentManager& manager, sf::RenderWindow& window, Entity::ID entity1, Entity::ID entity2) {
	HealthSystem healthSystem(projectilePool, gameManager);  // Instantiate the HealthSystem to apply damage

	if (manager.getComponent<Velocity>(entity1) && manager.getComponent<Renderable>(entity1)->shape->getFillColor() == sf::Color::Red) {  // Regular projectile collision
		if (manager.getComponent<Health>(entity2)) {  // Assuming base has Health component
			healthSystem.applyDamage(manager, entity2, 1);  // Apply damage to base
		}
		manager.setEntityInUse(entity1, false);  // Deactivate entity1 (projectile)
		projectilePool.release(entity1);
	}
	else if (manager.getComponent<Velocity>(entity1) && manager.getComponent<Renderable>(entity1)->shape->getFillColor() == sf::Color::Green) {  // Green Power-up collision
		if (entity2 == 0) {  // Assuming entity2 is the player
			IncreaseRotationSpeedCommand increaseRotationSpeed;
			increaseRotationSpeed.execute(manager, 0);  // Execute command to increase rotation speed
		}
		manager.setEntityInUse(entity1, false);  // Deactivate the power-up after use
		projectilePool.release(entity1);
	}
	else if (manager.getComponent<Velocity>(entity1) && manager.getComponent<Renderable>(entity1)->shape->getFillColor() == sf::Color::Magenta) {  // Magenta Power-up collision
		if (entity2 == 0) {  // Assuming entity2 is the player
			Renderable* playerRender = manager.getComponent<Renderable>(0);  // Assuming player entity ID is 0
			if (playerRender) {
				playerRender->shape->setScale(1.5f * playerRender->shape->getScale().x, 1.5f * playerRender->shape->getScale().y);  // Increase diameter by 1.5x
			}
			scalePlayerRotation(manager, window);
			scalePlayerCollider(manager);
		}
		manager.setEntityInUse(entity1, false);  // Deactivate the power-up after use
		projectilePool.release(entity1);
	}
	// Handle entity2 as well
	if (manager.getComponent<Velocity>(entity2) && manager.getComponent<Renderable>(entity2)->shape->getFillColor() == sf::Color::Red) {
		if (manager.getComponent<Health>(entity1)) {
			healthSystem.applyDamage(manager, entity1, 1);
		}
		manager.setEntityInUse(entity2, false);
		projectilePool.release(entity2);
	}
	else if (manager.getComponent<Velocity>(entity2) && manager.getComponent<Renderable>(entity2)->shape->getFillColor() == sf::Color::Green) {
		if (entity1 == 0) {
			IncreaseRotationSpeedCommand increaseRotationSpeed;
			increaseRotationSpeed.execute(manager, 0);
		}
		manager.setEntityInUse(entity2, false);
		projectilePool.release(entity2);
	}
	else if (manager.getComponent<Velocity>(entity2) && manager.getComponent<Renderable>(entity2)->shape->getFillColor() == sf::Color::Magenta) {
		if (entity1 == 0) {
			Renderable* playerRender = manager.getComponent<Renderable>(0);  // Assuming player entity ID is 0
			if (playerRender) {
				playerRender->shape->setScale(1.5f * playerRender->shape->getScale().x, 1.5f * playerRender->shape->getScale().y);  // Increase diameter by 1.5x
			}
			scalePlayerRotation(manager, window);
			scalePlayerCollider(manager);
		}
		manager.setEntityInUse(entity2, false);
		projectilePool.release(entity2);
	}
}

void CollisionSystem::scalePlayerCollider(ComponentManager& manager) {
	Entity::ID playerId = 0;  // Assuming player entity ID is 0

	// Retrieve player components
	Renderable* playerRender = manager.getComponent<Renderable>(playerId);
	BoxCollider* playerBoxCollider = manager.getComponent<BoxCollider>(playerId);
	CircleCollider* playerCircleCollider = manager.getComponent<CircleCollider>(playerId);

	if (playerRender) {
		// Retrieve the global bounds of the player shape
		sf::FloatRect bounds = playerRender->shape->getGlobalBounds();

		// Update BoxCollider if it exists
		if (playerBoxCollider) {
			// Update collider size to match the scaled player shape
			playerBoxCollider->bounds.width = bounds.width;
			playerBoxCollider->bounds.height = bounds.height;

			// Update collider position based on the new size for a box
			playerBoxCollider->bounds.left = bounds.left;
			playerBoxCollider->bounds.top = bounds.top;
		}

		// Update CircleCollider if it exists
		if (playerCircleCollider) {
			// Set the new radius as half of the largest dimension (width or height)
			playerCircleCollider->radius = std::max(bounds.width, bounds.height) / 2;

			// Update the circle collider's centre
			playerCircleCollider->center.x = bounds.left + bounds.width / 2;
			playerCircleCollider->center.y = bounds.top + bounds.height / 2;
		}
	}
}

void CollisionSystem::scalePlayerRotation(ComponentManager& manager, sf::RenderWindow& window) {
	Entity::ID playerId = 0;  // Assuming player entity ID is 0

	Rotation* playerRotation = manager.getComponent<Rotation>(playerId);
	Renderable* playerRender = manager.getComponent<Renderable>(playerId);
	BoxCollider* playerBoxCollider = manager.getComponent<BoxCollider>(playerId);
	CircleCollider* playerCircleCollider = manager.getComponent<CircleCollider>(playerId);

	if (playerRender) {
		// Retrieve the global bounds of the player shape
		sf::FloatRect bounds = playerRender->shape->getLocalBounds();//getGlobalBounds();

		// Update BoxCollider if it exists
		if (playerBoxCollider) {
			// Update rotation centre based on new size for a box
			playerRotation->centerX = window.getSize().x / 2 - playerBoxCollider->bounds.width * 3 / 4;
			playerRotation->centerY = window.getSize().y / 2 - playerBoxCollider->bounds.height * 3 / 4;

			// Update minimum rotation radius based on the largest dimension
			playerRotation->minRadius = dynamic_cast<sf::CircleShape*>(manager.getComponent<Renderable>(1)->shape)->getRadius()
				+ playerBoxCollider->bounds.width * 3 / 4;
		}

		// Update CircleCollider if it exists
		if (playerCircleCollider) {
			// Update the circle collider's centre
			playerCircleCollider->center.x = bounds.left + bounds.width / 2;
			playerCircleCollider->center.y = bounds.top + bounds.height / 2;

			// Update rotation centre based on new size for a circle
			playerRotation->centerX = playerCircleCollider->center.x;
			playerRotation->centerY = playerCircleCollider->center.y;
		}
	}
}

void ProjectileSpawnSystem::update(ComponentManager& manager, sf::RenderWindow& window, float deltaTime) {
	elapsedTime += deltaTime;

	if (elapsedTime >= timeWindow) {
		elapsedTime = 0.f;

		// Generate a random number for power-up spawn
		std::uniform_int_distribution<> distrib(0, projectilesRemaining - 1);  // Define the range
		int randomNum = distrib(gen);

		// Decide randomly if we spawn a power-up or a regular projectile
		if (!powerUpSpawned && randomNum == 0) {  // First projectile of the level TESTTESTTEST
			spawnPowerUp(manager, window);
		}
		else {
			spawnProjectile(manager, window);
		}

		projectilesRemaining--;

		// Check if it's time to reduce the time window (i.e., move to the next level)
		if (projectilesRemaining == 0) {
			nextLevel();
		}
	}
}

void ProjectileSpawnSystem::spawnPowerUp(ComponentManager& manager, sf::RenderWindow& window) {
	// Randomly choose between speed or size power-up
	std::uniform_int_distribution<> distrib(0, 1);
	int powerUpType = distrib(gen);
	if (powerUpType == 0) {
		launchSpeedPowerUp(manager, window);  // Green power-up
	}
	else {
		launchSizePowerUp(manager, window);  // Magenta power-up
	}
	powerUpSpawned = true;
	std::cout << "Power-up spawned!" << std::endl;  // Debug message
}

void ProjectileSpawnSystem::spawnProjectile(ComponentManager& manager, sf::RenderWindow& window) {
	launchProjectile(manager, window);
	std::cout << "Projectile spawned!" << std::endl;  // Debug message
}

void ProjectileSpawnSystem::nextLevel() {
	powerUpSpawned = false;
	level++;
	levelTime = 10 * initialTimeWindow;
	timeWindow *= 0.9f;  // Reduce the time window for the next level  // Maintain total level time consistency
	totalProjectiles = static_cast<int>(levelTime / timeWindow);  // Calculate the new projectile count
	projectilesRemaining = totalProjectiles;  // Reset projectile count for the next level
}

void ProjectileSpawnSystem::launchProjectile(ComponentManager& manager, sf::RenderWindow& window) {
	Entity* projectile = projectilePool.acquire();
	if (!projectile) return;

	// Correctly use manager instead of componentManager
	manager.removeComponent<Transform>(projectile->getId());
	manager.removeComponent<Velocity>(projectile->getId());
	manager.removeComponent<Renderable>(projectile->getId());
	manager.removeComponent<BoxCollider>(projectile->getId());

	// Initialize the projectile with new components
	sf::Vector2f spawnPosition = getRandomEdgePosition(window);
	manager.addComponent<Transform>(projectile->getId(), Transform(spawnPosition.x, spawnPosition.y, 0.f));

	float centerX = window.getSize().x / 2.f;
	float centerY = window.getSize().y / 2.f;
	float dx = centerX - spawnPosition.x;
	float dy = centerY - spawnPosition.y;
	float magnitude = std::sqrt(dx * dx + dy * dy);

	// Set velocity towards the center
	manager.addComponent<Velocity>(projectile->getId(), Velocity((dx / magnitude) * 100.f, (dy / magnitude) * 100.f));

	sf::CircleShape* shape = new sf::CircleShape(5.f);
	shape->setFillColor(sf::Color::Red);
	manager.addComponent<Renderable>(projectile->getId(), Renderable(shape));
	manager.addComponent<BoxCollider>(projectile->getId(), BoxCollider(spawnPosition.x, spawnPosition.y, shape->getRadius() * 2, shape->getRadius() * 2));
}

sf::Vector2f ProjectileSpawnSystem::getRandomEdgePosition(sf::RenderWindow& window) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<> disX(0, window.getSize().x);
	static std::uniform_real_distribution<> disY(0, window.getSize().y);

	int edge = rand() % 4;
	switch (edge) {
	case 0: return sf::Vector2f(disX(gen), 0);  // Top edge
	case 1: return sf::Vector2f(window.getSize().x, disY(gen));  // Right edge
	case 2: return sf::Vector2f(disX(gen), window.getSize().y);  // Bottom edge
	case 3: return sf::Vector2f(0, disY(gen));  // Left edge
	}
	return sf::Vector2f(0, 0);  // Default case
}

void ProjectileSpawnSystem::launchSpeedPowerUp(ComponentManager& manager, sf::RenderWindow& window) {
	Entity* powerUp = projectilePool.acquire();
	if (!powerUp) return;

	// Clear existing components for the reused power-up
	manager.removeComponent<Transform>(powerUp->getId());
	manager.removeComponent<Velocity>(powerUp->getId());
	manager.removeComponent<Renderable>(powerUp->getId());
	manager.removeComponent<BoxCollider>(powerUp->getId());

	// Initialize the power-up with new components
	sf::Vector2f spawnPosition = getRandomEdgePosition(window);
	manager.addComponent<Transform>(powerUp->getId(), Transform(spawnPosition.x, spawnPosition.y, 0.f));

	// Set a slower velocity for the power-up towards the center
	float centerX = window.getSize().x / 2.f;
	float centerY = window.getSize().y / 2.f;
	float dx = centerX - spawnPosition.x;
	float dy = centerY - spawnPosition.y;
	float magnitude = std::sqrt(dx * dx + dy * dy);

	// Set velocity towards the center (slower speed for power-up)
	manager.addComponent<Velocity>(powerUp->getId(), Velocity((dx / magnitude) * 50.f, (dy / magnitude) * 50.f));

	sf::CircleShape* shape = new sf::CircleShape(5.f);
	shape->setFillColor(sf::Color::Green);  // Different colour for power-up
	manager.addComponent<Renderable>(powerUp->getId(), Renderable(shape));
	manager.addComponent<BoxCollider>(powerUp->getId(), BoxCollider(spawnPosition.x, spawnPosition.y, shape->getRadius() * 2, shape->getRadius() * 2));
}

void ProjectileSpawnSystem::launchSizePowerUp(ComponentManager& manager, sf::RenderWindow& window) {
	Entity* powerUp = projectilePool.acquire();
	if (!powerUp) return;

	// Clear existing components for the reused power-up
	manager.removeComponent<Transform>(powerUp->getId());
	manager.removeComponent<Velocity>(powerUp->getId());
	manager.removeComponent<Renderable>(powerUp->getId());
	manager.removeComponent<BoxCollider>(powerUp->getId());

	// Initialize the power-up with new components
	sf::Vector2f spawnPosition = getRandomEdgePosition(window);
	manager.addComponent<Transform>(powerUp->getId(), Transform(spawnPosition.x, spawnPosition.y, 0.0f));

	// Set a slower velocity for the power-up towards the center
	float centerX = window.getSize().x / 2.0f;
	float centerY = window.getSize().y / 2.0f;
	float dx = centerX - spawnPosition.x;
	float dy = centerY - spawnPosition.y;
	float magnitude = std::sqrt(dx * dx + dy * dy);

	// Set velocity towards the center (slower speed for power-up)
	manager.addComponent<Velocity>(powerUp->getId(), Velocity((dx / magnitude) * 50.0f, (dy / magnitude) * 50.0f));

	sf::CircleShape* shape = new sf::CircleShape(5.f);
	shape->setFillColor(sf::Color::Magenta);  // Different color for magenta power-up
	manager.addComponent<Renderable>(powerUp->getId(), Renderable(shape));
	manager.addComponent<BoxCollider>(powerUp->getId(), BoxCollider(spawnPosition.x, spawnPosition.y, shape->getRadius() * 2, shape->getRadius() * 2));
}

void ProjectileSpawnSystem::reset(ComponentManager& manager) {
	std::cout << "Resetting Projectile Spawn System..." << std::endl;
	auto entitiesWithVelocity = manager.getEntitiesWithComponents<Velocity>();
	for (auto entity : entitiesWithVelocity) {
		manager.setEntityInUse(entity, false);  // Deactivate all projectiles
		projectilePool.release(entity);  // Correctly release the entity back to the pool
	}
	timeWindow = initialTimeWindow;  // Reset time window to initial value
	elapsedTime = 0.f;  // Reset elapsed time
	level = 1;  // Reset level
	totalProjectiles = 10;  // Reset to initial projectile count
	projectilesRemaining = totalProjectiles;  // Reset remaining projectiles
	levelTime = totalProjectiles * initialTimeWindow;  // Recalculate level time
}

void HealthSystem::applyDamage(ComponentManager& manager, Entity::ID entity, int damage) {
	Health* health = manager.getComponent<Health>(entity);
	if (health) {
		health->currentHealth -= damage;

		std::cout << "Damage applied to entity " << entity << ": -" << damage << " health. Current Health: " << health->currentHealth << std::endl;
	}
}

void HealthSystem::update(ComponentManager& manager, sf::RenderWindow& window) {
	auto entities = manager.getEntitiesWithComponents<Health>();

	for (auto entity : entities) {
		if (!manager.isEntityInUse(entity)) continue;

		Health* health = manager.getComponent<Health>(entity);

		if (health && health->currentHealth <= 0) {
			// If health is 0 or less, it's game over.
			gameManager.onBaseHealthDepleted(manager, window);
			break; // Exit after resetting the game.
		}
	}
}

void GameManager::resetGame(ComponentManager& manager, sf::RenderWindow& window) {
	std::cout << "Game Over! Resetting game..." << std::endl;

	// Reset player
	Entity::ID playerEntity = 0;
	Renderable* playerRender = manager.getComponent<Renderable>(playerEntity);
	if (playerRender) {
		playerRender->shape->setScale(1.f, 1.f);  // Reset player scale
	}

	Rotation* playerRotation = manager.getComponent<Rotation>(playerEntity);
	if (playerRotation && playerRender) {
		// Cast shape to sf::CircleShape and calculate new positions
		sf::CircleShape* circleShape = dynamic_cast<sf::CircleShape*>(playerRender->shape);
		if (circleShape) {
			float radius = circleShape->getRadius();
			float x = window.getSize().x / 2 - radius;
			float y = window.getSize().y / 2 - radius;
			float maxRadius = window.getSize().x / 2 - radius;

			// Update existing Rotation component
			playerRotation->angle = 0.f;
			playerRotation->speed = playerRotation->startSpeed;
			playerRotation->centerX = x;
			playerRotation->centerY = y;
			playerRotation->radius = window.getSize().x / 4;
			playerRotation->maxRadius = maxRadius;
			playerRotation->clockwise = true;
		}
	}

	// Reset base
	Entity::ID baseEntity = 1;
	Health* baseHealth = manager.getComponent<Health>(baseEntity);
	if (baseHealth) {
		baseHealth->currentHealth = baseHealth->maxHealth;
	}

	if (playerRotation && playerRender) {
		sf::CircleShape* baseShape = dynamic_cast<sf::CircleShape*>(manager.getComponent<Renderable>(baseEntity)->shape);
		sf::CircleShape* playerShape = dynamic_cast<sf::CircleShape*>(playerRender->shape);
		if (baseShape && playerShape) {
			playerRotation->minRadius = baseShape->getRadius() + playerShape->getRadius();
		}
	}

	// Reset the projectile spawn system's time window
	projectileSpawnSystem.reset(manager);
	collisionSystem.scalePlayerCollider(manager);

	// Reset the game window or any other game-specific logic
	window.clear();
}

void GameManager::onBaseHealthDepleted(ComponentManager& manager, sf::RenderWindow& window) {
	// When base health is 0, reset the game
	resetGame(manager, window);
}