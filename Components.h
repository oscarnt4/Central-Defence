#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

struct Component {
	virtual ~Component() = default;
};

struct Velocity : public Component {
	float dx, dy;

	Velocity(float dx = 0.f, float dy = 0.f)
		: dx(dx), dy(dy) {}

	virtual ~Velocity() override = default;
};

struct Rotation : public Component {
	float angle;
	float speed;
	float startSpeed;
	bool clockwise;
	float centerX;
	float centerY;
	float radius;
	float maxRadius;
	float minRadius;

	Rotation(float angle = 0.f, float startSpeed = 0.f, bool clockwise = true,
		float centerX = 0.f, float centerY = 0.f, float radius = 0.f,
		float maxRadius = 0.f, float minRadius = 0.f)
		: angle(angle), speed(startSpeed), startSpeed(startSpeed), clockwise(clockwise),
		centerX(centerX), centerY(centerY), radius(radius), maxRadius(maxRadius),
		minRadius(minRadius) {}

	virtual ~Rotation() override = default;

	void increaseRadius(float amount) {
		radius += amount;
		if (radius > maxRadius) {
			radius = maxRadius;
		}
	}

	void decreaseRadius(float amount) {
		radius -= amount;

		if (radius < minRadius) {
			radius = minRadius;
		}
	}
};

struct Health : public Component{
	int currentHealth;
	int maxHealth;

	Health(int maxHealth) : currentHealth(maxHealth), maxHealth(maxHealth) {}

	virtual ~Health() override = default;
};

struct Renderable : public Component {
	sf::Shape* shape;
	sf::Sprite* sprite; // Optional: If using textures instead of simple shapes

	Renderable(sf::Shape* shape = nullptr, sf::Sprite* sprite = nullptr)
		: shape(shape), sprite(sprite) {}

	virtual ~Renderable() override = default;
};

struct Transform : public Component {
	float x, y;
	float angle;

	Transform(float x = 0.f, float y = 0.f, float angle = 0.f)
		: x(x), y(y), angle(angle) {}

	virtual ~Transform() override = default;
};

struct Collider : public Component {
	virtual ~Collider() = default;
};

// Circle collider for base
struct CircleCollider : public Collider {
	sf::Vector2f center;  // Center position of the circle collider
	float radius;         // Radius of the circle

	CircleCollider(float x = 0.f, float y = 0.f, float r = 0.f)
		: center(x, y), radius(r) {}
};

// Box collider for all other entities with a collider (since they are mostly small)
struct BoxCollider : public Collider {
	sf::FloatRect bounds;  // Bounding box for collider

	BoxCollider(float x = 0.f, float y = 0.f, float width = 0.f, float height = 0.f)
		: bounds(x, y, width, height) {}
};