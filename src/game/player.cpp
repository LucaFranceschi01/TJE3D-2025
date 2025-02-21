//
// Created by Xavi Cañadas on 6/2/25.
//

#include "player.h"
#include "world.h"
#include "framework/camera.h"

#include "framework/input.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "game/game.h"

Player::Player(Mesh* mesh, const Material& material, const std::string& name) : EntityMesh(mesh, material, name)
{
    this->mesh = mesh;
    this->material = material;
    this->name = name;
}


void Player::init()
{
    life = 3;
    model.setTranslation(0, 25, 0);
    // add more
}

void Player::render(Camera* camera)
{
    // Render entity
    EntityMesh::render(camera);

    // render life text. todo: es temporal
    std::string life_text = "lifes: " + std::to_string(life) + "/3";
    drawText(700, 2, life_text, Vector3(1, 1, 1), 2);

    if (Game::instance->debug_view) {
        renderDebug(camera);
    }
}



void Player::update(float dt)
{

    if (life <= 0) Game::instance->goToStage(MAIN_MENU_ST);

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
    testCollisions(position, dt);

    // update player position
    position += velocity * dt;

    yaw = positive_modulo(yaw, 360.f*DEG2RAD);
    pitch = positive_modulo(pitch, 360.f*DEG2RAD);

    model.setTranslation(position);
    model.rotate(yaw, World::front);
    model.rotate(pitch, World::right);
    
    // decrease when not moving
    velocity.x *= 0.9f;
    velocity.z *= 0.9f;

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

    // calls test_scene_collision to check if the new position collides with something.
    World::getInstance()->test_scene_collisions(position + velocity * dt, collisions, ground_collisions);


    // ground collisions
    bool is_grounded = false;
    collision_fluid = false;

    // enviroment collisions
    for (const sCollisionData& collision_data : collisions) {

        // if normal is pointing upwards, it means it's a floor collision
        float up_factor = fabs(collision_data.col_normal.dot(Vector3::UP));


        switch (collision_data.collider->layer) {
            case GROUND: {
                if (up_factor > 0.8f) {
                    is_grounded = true;
                    // If we're falling, stop at ground level
                    if (velocity.y < 0) {
                        position.y = collision_data.col_point.y;
                        velocity.y = 0.0f;
                    }
                }
                break;
            }
            case OBSTACLE: {
                // quit one life
                life--;

                // send the object to delete
                World::getInstance()->destroyEntity(collision_data.collider);
                break;
            }

            case FLUID: {
                // todo: no detecta las colisiones con el objeto fluido
                collision_fluid = true;
                break;
            }


            default:
                break;
        }

    }

    float gravity = 100.0f;
    float jump_force = 1.f;


    // this part is to make the jump fluid
    if (jump_time > 0) {
        jump_time -= dt;
        velocity.y += jump_force * jump_time;
    }

    if (!is_grounded && jump_time <= 0) {
        velocity.y += -gravity * dt;
        jump_time = 0;
    }
    // Handle jumping with better feel
    else if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
        jump_time = 0.25f;
    }

    collision = (!collisions.empty());
}

void Player::renderDebug(Camera* camera)
{
    float sphere_radious = 1.f;

    Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
    Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");
    Matrix44 m = model;

    shader->enable();

    {

        Vector4 color;

        if (collision_fluid) {
            color = Vector4(0.0f, 1.0f, 1.0f, 1.0f);

        } else if (collision) {
            color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
        } else {
            color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        }


        m.scale(sphere_radious, sphere_radious, sphere_radious);
        shader->setUniform("u_color", color);
        shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
        shader->setUniform("u_model", m);

        mesh->render(GL_LINES);
    }

    shader->disable();
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