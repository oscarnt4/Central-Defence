#pragma once

class Entity {
public:
    using ID = unsigned int;

    explicit Entity(ID id) : id(id), inUse(false) {}

    ID getId() const { return id; }

    bool inUse;

private:
    ID id;
};
