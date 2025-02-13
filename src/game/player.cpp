//
// Created by Xavi Cañadas on 6/2/25.
//

#include "player.h"
#include "world.h"

#include "framework/input.h"

Player::Player(Mesh *mesh, const Material &material, const std::string &name) {
    this->mesh = mesh;
    this->material = material;
    this->name = name;
}

void Player::update(float dt)
{
    //float camera_yaw = World::getInstance()->camera_yaw;

    //Matrix44 mYaw;
    //mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));

    Vector3 front = model.frontVector();
    Vector3 right = model.rightVector();

    Vector3 position = model.getTranslation();

    Vector3 move_dir;

    if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) move_dir += front * dt;
    if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) move_dir -= front * dt;
    if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) yaw -= 5.f * dt;
    if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) yaw += 5.f * dt;

    float speed_mult = walk_speed;

    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
        speed_mult *= 0.3;

    move_dir.normalize();
    move_dir *= speed_mult;

    velocity += move_dir;

    // mirar colisions aquí

    // update player position
    position += velocity * dt;

    // decrease when not moving
    velocity.x *= 0.5f;
    velocity.z *= 0.5f;

    model.setTranslation(position);
    model.rotate(yaw, Vector3(0.0f, 1.0f, 0.0f));

    // super->update
    EntityMesh::update(dt);
}
