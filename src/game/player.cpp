//
// Created by Xavi Cañadas on 6/2/25.
//

#include "player.h"
#include "world.h"

#include "framework/input.h"

Player::Player(Mesh *mesh, const Material &material, const std::string &name) {
}

void Player::update(float dt) {
    float camera_yaw = World::getInstance()->camera_yaw;

    Matrix44 mYaw;
    mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));

    Vector3 front = mYaw.frontVector();
    Vector3 right = mYaw.rightVector();

    Vector3 position = model.getTranslation();

    Vector3 move_dir;

    if (Input:: isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP))
        move_dir += front;

    if (Input:: isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN))
        move_dir -= front;

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
    model.setRotation(camera_yaw, Vector3(0, 1, 0));

    // super->update
    EntityMesh::update(dt);
}
