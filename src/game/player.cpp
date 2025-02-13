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
    Vector3 front = model.rightVector(); // TODO: TAKE INTO ACCOUNT THAT THESE CHANGE, MAKE WORLD ONES THAT DO NOT CHANGE AND USE THEM IN CAMERA UPDATE ALSO
    Vector3 right = model.frontVector();

    Vector3 position = model.getTranslation();

    Vector3 move_dir;

    bool pressing_button = false;

    if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) {
        move_dir += front * dt;
        pitch += 5.f * dt;
        pressing_button = true;
    }
    if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
        move_dir -= front * dt;
        pitch -= 5.f * dt;
        pressing_button = true;
    }
    if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
        move_dir -= right * dt;
        yaw -= 5.f * dt;
        pressing_button = true;
    }
    if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
        move_dir += right * dt;
        yaw += 5.f * dt;
        pressing_button = true;
    }

    float speed_mult = walk_speed;

    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
        speed_mult *= 0.3;

    move_dir.normalize();
    move_dir *= speed_mult;

    // back up to calculate yaw and then update
    Vector3 old_velocity = velocity;
    velocity += move_dir;

    // mirar colisions aquí

    // update player position
    position += velocity * dt;

    model.setTranslation(position);
    model.rotate(yaw, Vector3::UP);
    //model.rotate(pitch, Vector3(0.f, 0.f, 1.f));
    
    // decrease when not moving
    if (!pressing_button) {
        velocity *= 0.5f;
        yaw *= 0.99f;
        pitch *= 0.99f;
    }

    // super->update
    EntityMesh::update(dt);
}
