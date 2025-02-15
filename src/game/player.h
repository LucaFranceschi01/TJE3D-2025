//
// Created by Xavi Cañadas on 6/2/25.
//

#pragma once

#include "framework/entities/entityMesh.h"


class Player : public EntityMesh
{
private:
    float yaw = 0.f;
    float pitch = 0.f;
    float walk_speed = 1.f;
    float rotational_speed = 30.f;
    Vector3 velocity = Vector3(0.f);

public:
    Player() = default;
    Player(Mesh* mesh, const Material& material, const std::string& name = "");

    void update(float dt) override;
};

static void dampen(float* deg);
inline static float positive_modulo(float i, float n);
