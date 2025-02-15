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
    //Vector3 front = model.rightVector(); // TODO: TAKE INTO ACCOUNT THAT THESE CHANGE, MAKE WORLD ONES THAT DO NOT CHANGE AND USE THEM IN CAMERA UPDATE ALSO
    //Vector3 right = model.frontVector();
    
    Vector3 front = World::front;
    Vector3 right = World::right;

    Vector3 position = model.getTranslation();

    Vector3 move_dir;

    bool pressing_button = false;

    if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) {
        move_dir += front * dt;
        pitch += rotational_speed * dt;
        pressing_button = true;
    }
    if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
        move_dir -= front * dt;
        pitch -= rotational_speed * dt;
        pressing_button = true;
    }
    if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
        move_dir -= right * dt;
        yaw += rotational_speed * dt;
        pressing_button = true;
    }
    if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
        move_dir += right * dt;
        yaw -= rotational_speed * dt;
        pressing_button = true;
    }

    float speed_mult = walk_speed;

    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
        speed_mult *= 0.3f;

    move_dir.normalize();
    move_dir *= speed_mult;

    velocity += move_dir;

    // mirar colisions aquí

    // update player position
    position += velocity * dt;

    yaw = positive_modulo(yaw, 360.f*DEG2RAD);
    pitch = positive_modulo(pitch, 360.f*DEG2RAD);

    model.setTranslation(position);
    model.rotate(yaw, World::front);
    model.rotate(pitch, World::right);
    
    // decrease when not moving
    velocity *= 0.9f;

    if (move_dir.z == 0.f) {
        dampen(&yaw, dt);
    }
    if (move_dir.x == 0.f) {
        dampen(&pitch, dt);
    }
    
    // super->update
    EntityMesh::update(dt);
}

static void dampen(float* deg) {
    if (*deg > 180.f*DEG2RAD) {
        *deg *= 1.01f;
    }
    else {
        *deg *= 0.99f;
    }
}

inline static float positive_modulo(float i, float n) {
    // props to https://stackoverflow.com/a/14997413
    return std::fmod((std::fmod(i,n) + n), n);
}