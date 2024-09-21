#include "Commands.h"
#include "Components.h"

void RotateClockwiseCommand::execute(ComponentManager& manager, Entity::ID entity) {
    Rotation* rotation = manager.getComponent<Rotation>(entity);
    if (rotation) {
        rotation->clockwise = true;
    }
}

void RotateAntiClockwiseCommand::execute(ComponentManager& manager, Entity::ID entity) {
    Rotation* rotation = manager.getComponent<Rotation>(entity);
    if (rotation) {
        rotation->clockwise = false;
    }
}

void ChangeRotationDirectionCommand::execute(ComponentManager& manager, Entity::ID entity) {
    Rotation* rotation = manager.getComponent<Rotation>(entity);
    if (rotation) {
        rotation->clockwise = !rotation->clockwise;
    }
}

void IncreaseRadiusCommand::execute(ComponentManager& manager, Entity::ID entity) {
    Rotation* rotation = manager.getComponent<Rotation>(entity);
    if (rotation) {
        rotation->increaseRadius(4.f);
    }
}

void DecreaseRadiusCommand::execute(ComponentManager& manager, Entity::ID entity) {
    Rotation* rotation = manager.getComponent<Rotation>(entity);
    if (rotation) {
        rotation->decreaseRadius(4.f);
    }
}

void IncreaseRotationSpeedCommand::execute(ComponentManager& manager, Entity::ID entity) {
    Rotation* rotation = manager.getComponent<Rotation>(entity);
    if (rotation) {
        rotation->speed += 50.f;
    }
}
