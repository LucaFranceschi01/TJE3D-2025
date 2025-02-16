//
// Created by Xavi Cañadas on 6/2/25.
//

#pragma once

#include <sstream>

#include "framework/entities/entityMesh.h"


class Player : public EntityMesh
{
private:
    float yaw = 0.f;
    float pitch = 0.f;
    float walk_speed = 15.f;
    Vector3 velocity = Vector3(0.f);
    Vector3 move_dir; // lo he puesto a aqui porque queria imprimirlo en la pantalla para probar cosas

public:
    Player() = default;
    Player(Mesh* mesh, const Material& material, const std::string& name = "");

    void render(Camera* camera) override;
    void update(float dt) override;
    void testCollisions(Vector3 position, float dt);

};
