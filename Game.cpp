#include "Game.h"
#include "Commands.h"

Game::Game()
	: mWindow(sf::VideoMode(800, 800), "Central Defence"),
	projectilePool(100), // Initialise projectilePool
	projectileSpawnSystem(projectilePool, 4.f), // Initialise projectileSpanSystem
	gameManager(projectileSpawnSystem, collisionSystem),  // Initialise gameManager
	healthSystem(projectilePool, gameManager),  // Initialise healthSystem
	collisionSystem(projectilePool, gameManager)  // Initialise collisionSystem
{
	initialisePlayer();
	initialiseBase();

	// Update player minimum rotation radius
	Rotation* playerRotation = componentManager.getComponent<Rotation>(playerEntity);
	playerRotation->minRadius =
		dynamic_cast<sf::CircleShape*>(componentManager.getComponent<Renderable>(baseEntity)->shape)->getRadius()
		+ dynamic_cast<sf::CircleShape*>(componentManager.getComponent<Renderable>(playerEntity)->shape)->getRadius();
}

void Game::run() {
	const float desiredFPS = 60.f;
	const float timePerFrame = 1.f / desiredFPS;

	sf::Clock clock;

	while (mWindow.isOpen()) {
		gameLoop(clock, timePerFrame);
	}
}

void Game::gameLoop(sf::Clock& clock, float timePerFrame) {
	sf::Time elapsed = clock.restart();
	float deltaTime = elapsed.asSeconds();

	processInput();
	update(deltaTime);
	render();

	float frameTime = clock.getElapsedTime().asSeconds();

	if (frameTime < timePerFrame) {
		sf::sleep(sf::seconds(timePerFrame - frameTime)); // Wait for full frame time before next loop
	}
}

void Game::processInput() {
	RotateClockwiseCommand rotateClockwise;
	RotateAntiClockwiseCommand rotateAntiClockwise;
	ChangeRotationDirectionCommand changeRotationDirection;
	IncreaseRadiusCommand increaseRadius;
	DecreaseRadiusCommand decreaseRadius;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		rotateClockwise.execute(componentManager, playerEntity);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		rotateAntiClockwise.execute(componentManager, playerEntity);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		increaseRadius.execute(componentManager, playerEntity);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		decreaseRadius.execute(componentManager, playerEntity);
	}

	sf::Event event;
	while (mWindow.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			mWindow.close();
	}
}

void Game::update(float deltaTime) {
	movementSystem.update(componentManager, deltaTime);
	rotationSystem.update(componentManager, deltaTime);
	collisionSystem.update(componentManager, mWindow);
	projectileSpawnSystem.update(componentManager, mWindow, deltaTime);
	healthSystem.update(componentManager, mWindow);
}

void Game::render() {
	mWindow.clear();
	updateBaseColour();
	renderSystem.render(componentManager, mWindow);
	debug.renderColliders(componentManager, mWindow);
	mWindow.display();
}

void Game::initialisePlayer() {
	Entity player = Entity(0);
	componentManager.addComponent<Transform>(player.getId(), Transform(0.f, 0.f, 0.f));
	sf::CircleShape* playerShape = new sf::CircleShape(10.f);
	playerShape->setFillColor(sf::Color::Cyan);
	componentManager.addComponent<Renderable>(player.getId(), Renderable(playerShape, nullptr));
	float x = mWindow.getSize().x / 2 - playerShape->getRadius();
	float y = mWindow.getSize().y / 2 - playerShape->getRadius();
	float maxRadius = mWindow.getSize().x / 2 - playerShape->getRadius();
	componentManager.addComponent<Rotation>(player.getId(), Rotation(0.f, 80.f, true, x, y, mWindow.getSize().x / 4, maxRadius, 1.f));
	componentManager.addComponent<BoxCollider>(player.getId(), BoxCollider(x, y, playerShape->getRadius() * 2, playerShape->getRadius() * 2));
	playerEntity = player.getId();
}

void Game::initialiseBase() {
	Entity base = Entity(1);
	sf::CircleShape* baseShape = new sf::CircleShape(100.f);
	baseShape->setFillColor(sf::Color::White);
	componentManager.addComponent<Renderable>(base.getId(), Renderable(baseShape, nullptr));
	float centerX = mWindow.getSize().x / 2 - baseShape->getRadius();
	float centerY = mWindow.getSize().y / 2 - baseShape->getRadius();
	componentManager.addComponent<Transform>(base.getId(), Transform(centerX, centerY, 0.f));
	componentManager.addComponent<CircleCollider>(base.getId(), CircleCollider(centerX + baseShape->getRadius(), centerY + baseShape->getRadius(), baseShape->getRadius()));
	componentManager.addComponent<Health>(base.getId(), Health(4));  // Add Health component with 4 max health
	baseEntity = base.getId();
}

void Game::initialiseProjectile(float startX, float startY, float velocityX, float velocityY) {
	// Get a projectile from the pool
	Entity* projectile = projectilePool.acquire();
	if (!projectile) return;  // If no available projectile, do nothing

	// Clear existing components
	componentManager.removeComponent<Transform>(projectile->getId());
	componentManager.removeComponent<Velocity>(projectile->getId());
	componentManager.removeComponent<Renderable>(projectile->getId());
	componentManager.removeComponent<BoxCollider>(projectile->getId());

	// Initialize projectile components
	componentManager.addComponent<Transform>(projectile->getId(), Transform(startX, startY, 0.f));
	componentManager.addComponent<Velocity>(projectile->getId(), Velocity(velocityX, velocityY));

	sf::CircleShape* shape = new sf::CircleShape(5.f);
	shape->setFillColor(sf::Color::Red);
	componentManager.addComponent<Renderable>(projectile->getId(), Renderable(shape));

	componentManager.addComponent<BoxCollider>(projectile->getId(), BoxCollider(startX, startY, shape->getRadius() * 2, shape->getRadius() * 2));
}

void Game::updateBaseColour() {
	// Get base Health
	Health* baseHealth = componentManager.getComponent<Health>(baseEntity);
	if (!baseHealth) return;

	// Get base current and max health
	int currentHealth = baseHealth->currentHealth;
	int maxHealth = baseHealth->maxHealth;

	// Calculate base colour based on health
	float healthRatio = static_cast<float>(currentHealth - 1) / (maxHealth - 1);
	int redIntensity = static_cast<int>((1.f - healthRatio) * 255);

	// Update the base colour
	sf::CircleShape* baseShape = dynamic_cast<sf::CircleShape*>(componentManager.getComponent<Renderable>(baseEntity)->shape);
	if (baseShape) {
		baseShape->setFillColor(sf::Color(255, 255 - redIntensity, 255 - redIntensity));  // Change to a shade of red
	}
}
