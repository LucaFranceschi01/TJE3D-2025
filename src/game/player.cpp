//
// Created by Xavi Cañadas on 6/2/25.
//

#include "player.h"
#include "world.h"

#include "framework/input.h"

Player::Player(Mesh* mesh, const Material& material, const std::string& name) : EntityMesh(mesh, material, name)
{
    this->mesh = mesh;
    this->material = material;
    this->name = name;
    model.setTranslation(0, 2, 0);
}

void Player::update(float dt)
{
    // If camera is in free mode, avoid moving the player
    if (World::getInstance()->camera_mode == World::e_camera_mode::FREE) return;
    
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
    //testCollisions(position, dt);

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
        dampen(&yaw);
    }
    if (move_dir.x == 0.f) {
        dampen(&pitch);
    }
    
    // super->update
    EntityMesh::update(dt);
}

void Player::testCollisions(Vector3 position, float dt)
{
    // Check collisions with the world entities
    std::vector<sCollisionData> collisions;
    std::vector<sCollisionData> ground_collisions;

    World::getInstance()->test_scene_collisions(position + velocity * dt, collisions, ground_collisions);

    // enviroment collisions
    for (const sCollisionData& collision : collisions) {
        // if normal is pointing upwards, it means it's a floor collision
        float up_factor = fabs(collision.col_normal.dot(Vector3::UP));
        if (up_factor > 0.8)
            continue;

        // move along wall when colliding
        Vector3 new_direction = velocity.dot(collision.col_normal) * collision.col_normal;
        // falta algo en esta multiplicacipn?
        velocity.x -= new_direction.x;
        velocity.z -= new_direction.z;
    }

    // ground collisions
    bool is_grounded = false;

    for (const sCollisionData& collision : ground_collisions) {
        // if normal is pointing upwards, it means it's a floor collision
        float up_factor = fabs(collision.col_normal.dot(Vector3::UP));
        if (up_factor > 0.8)
            is_grounded = true;
    }

    if (!is_grounded) {
        velocity.y -= 9.8f * dt;
    }
    else if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
        velocity.y = 3.f;
    }
    else {
        velocity.y = 0.0f;
    }
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