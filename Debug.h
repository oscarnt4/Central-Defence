#pragma once

#include <SFML/Graphics.hpp>
#include "ComponentManager.h"
#include <iostream>

class Debug {
public:
    void renderColliders(ComponentManager& manager, sf::RenderWindow& window) {
        auto boxEntities = manager.getEntitiesWithComponents<BoxCollider>();
        for (auto entity : boxEntities) {
            BoxCollider* collider = manager.getComponent<BoxCollider>(entity);

            if (!collider || !manager.isEntityInUse(entity)) {
                continue; // Skip entity if not in use
            }

            if (auto* boxCollider = dynamic_cast<BoxCollider*>(collider)) {
                // Render box collider
                sf::RectangleShape shape(sf::Vector2f(boxCollider->bounds.width, boxCollider->bounds.height));
                shape.setPosition(boxCollider->bounds.left, boxCollider->bounds.top);
                shape.setFillColor(sf::Color::Transparent);
                shape.setOutlineColor(sf::Color::Green);
                shape.setOutlineThickness(1.f);
                window.draw(shape);
            }
        }
        auto circleEntities = manager.getEntitiesWithComponents<CircleCollider>();
        for (auto entity : circleEntities) {
            CircleCollider* collider = manager.getComponent<CircleCollider>(entity);

            if (!collider || !manager.isEntityInUse(entity)) {
                continue;
            }
            if (auto* circleCollider = dynamic_cast<CircleCollider*>(collider)) {
                // Render circle collider
                sf::CircleShape shape(circleCollider->radius);
                shape.setPosition(circleCollider->center.x - circleCollider->radius, circleCollider->center.y - circleCollider->radius);
                shape.setFillColor(sf::Color::Transparent);
                shape.setOutlineColor(sf::Color::Green);
                shape.setOutlineThickness(1
                
                );
                window.draw(shape);
            }
        }
    }
};
