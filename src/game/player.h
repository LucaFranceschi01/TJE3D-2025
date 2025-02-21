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
    float walk_speed = 3.f;
    float rotational_speed = 30.f;
    float sphere_radious = 10.f;
    Vector3 velocity = Vector3(0.f);


    float jump_time = 0.0f;

    int life = 3;


public:

    bool collision = false;
    bool collision_fluid = false;

    Player() = default;
    Player(Mesh* mesh, const Material& material, const std::string& name = "");

    void init();
    void render(Camera* camera) override;
    void update(float dt) override;
    void testCollisions(Vector3 position, float dt);
    void renderDebug(Camera* camera);

};

static void dampen(float* deg);
inline static float positive_modulo(float i, float n);
