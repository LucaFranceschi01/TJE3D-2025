//
// Created by Xavi Cañadas on 6/2/25.
//

#include "player.h"

#include "game.h"
#include "world.h"

#include "framework/input.h"

Player::Player(Mesh* mesh, const Material& material, const std::string& name) : EntityMesh(mesh, material, name)
{
    model.setTranslation(0, 2, 0);


}

void Player::render(Camera* camera)
{
    drawText(Game::instance->window_width - 250, 2, "move dir: " + move_dir.to_string(), Vector3(1, 1, 1), 2);
    drawText(Game::instance->window_width - 250, 20, "velocity: " + velocity.to_string(), Vector3(1, 1, 1), 2);
    drawText(Game::instance->window_width - 250, 40, "position: " + model.getTranslation().to_string(), Vector3(1, 1, 1), 2);

    EntityMesh::render(camera);
}


void Player::update(float dt)
{
    if (World::getInstance()->camera_mode == World::e_camera_mode::FREE) return;
    // If camera is in free mode, avoid moving the player

    //Vector3 front = model.rightVector();
    // TODO: TAKE INTO ACCOUNT THAT THESE CHANGE, MAKE WORLD ONES THAT DO NOT CHANGE AND USE THEM IN CAMERA UPDATE ALSO
    //Vector3 right = model.frontVector();

    Vector3 front = Vector3(1, 0, 0);
    Vector3 right = Vector3(0, 0 ,1);

    Vector3 position = model.getTranslation();

    move_dir = Vector3();

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
    //testCollisions(position, dt);

    // update player position
    position += velocity * dt;


    Matrix44 translation_matrix = Matrix44();
    translation_matrix.setTranslation(position);

    Matrix44 rotation_matrix = Matrix44();
    rotation_matrix.setRotation(pitch, Vector3(0, 0, 1));

    model = rotation_matrix * translation_matrix;




    /*
    model.setTranslation(position);
    //model.rotate(yaw, Vector3::UP);
    //model.rotate(pitch, Vector3(0.f, 0.f, 1.f));
    */



    //Matrix44


    // decrease when not moving

    if (!pressing_button) {
        velocity *= 0.5f;
        //yaw *= 0.99f;
        //pitch *= 0.99f;
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


