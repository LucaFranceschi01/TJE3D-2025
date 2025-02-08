//
// Created by Xavi Cañadas on 6/2/25.
//

#pragma once

#include "framework/entities/entityMesh.h"

class Player : public EntityMesh {
private:
    float walk_speed = 0.0f;
    Vector3 velocity = Vector3(0.0f);

public:
    Player() = default;
    Player(Mesh* mesh, const Material& material, const std::string& name = "");

    void update(float dt) override;
};
