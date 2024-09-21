#pragma once
#include "ComponentManager.h"

class Command {
public:
    virtual ~Command() = default;
    virtual void execute(ComponentManager& manager, Entity::ID entity) = 0;
};

class IncreaseRadiusCommand : public Command {
public:
    void execute(ComponentManager& manager, Entity::ID entity) override;
};

class DecreaseRadiusCommand : public Command {
public:
    void execute(ComponentManager& manager, Entity::ID entity) override;
};

class RotateClockwiseCommand : public Command {
public:
    void execute(ComponentManager& manager, Entity::ID entity) override;
};

class RotateAntiClockwiseCommand : public Command {
public:
    void execute(ComponentManager& manager, Entity::ID entity) override;
};

class ChangeRotationDirectionCommand : public Command {
public:
    void execute(ComponentManager& manager, Entity::ID entity) override;
};

class IncreaseRotationSpeedCommand : public Command {
public:
    void execute(ComponentManager& manager, Entity::ID entity) override;
};