//
// Created by Xavi Cañadas on 6/2/25.
//

#include "player.h"
#include "game/world.h"
#include "game/stages/stage.h"

#include "framework/audio.h"
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

void Player::init(Vector3 pos)
{
    World::getInstance()->live = 3;
    model.setTranslation(pos);
}


void Player::render(Camera* camera)
{
    // Render entity
    EntityMesh::render(camera);


    if (Game::instance->debug_view) {

        const std::string normal_debug_str = "normal_orig: " + normal_orig.to_string();
        drawText(500, 16, normal_debug_str, Vector3(1, 1, 1), 2);

        const std::string front_debug_str = "front: " + front.to_string();
        drawText(550, 32, front_debug_str, Vector3(1, 1, 1), 2);

        const std::string velocity_debug_str = "velocity: " + velocity.to_string();
        drawText(500, 48, velocity_debug_str, Vector3(1, 1, 1), 2);

        renderDebug(camera);
    }
}



void Player::update(float dt)
{

    World* world_instance = World::getInstance();
    Vector3 position = model.getTranslation();


    if (world_instance->live <= 0 || position.y <= 0) {
        Audio::Play("data/sounds/sad_horn.wav");
        Game::instance->goToStage(MAIN_MENU_ST);
    }


    // If camera is in free mode, avoid moving the player
    if (world_instance->camera_mode == World::e_camera_mode::FREE) return;

    front = World::front;
    right = World::right;

    // mirar collisions aquí
    testCollisions(position, dt);


    Vector3 move_dir = Vector3(0.0f);

    moveControl(move_dir, dt);

    float speed_mult = walk_speed;

    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
        speed_mult *= 0.3f;

    move_dir.normalize();

    velocity += (move_dir * speed_mult);

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

void Player::moveControl(Vector3& move_dir, const float dt)
{
    World* world_instance = World::getInstance();
    if (world_instance->game_mode == World::RELEASE ||
        (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP))) {

        move_dir += front * dt;
        pitch += rotational_speed * dt;
        }
    if (world_instance->game_mode == World::DEBUG &&
        (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN))) {

        move_dir -= front * dt;
        pitch -= rotational_speed * dt;
        }
    if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
        move_dir -= right * dt;
        yaw += rotational_speed * dt;
    }
    if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
        move_dir += right * dt;
        yaw -= rotational_speed * dt;
    }
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

    // environment collisions
    for (const sCollisionData& collision_data : collisions) {
        switch (collision_data.collider->layer) {
            case GROUND:
            {
                normal_orig = collision_data.col_normal;

                Vector4 normal = Vector4(normal_orig.normalize(), 0.f);
                Matrix44 rot_mat;
                rot_mat.setRotation(PI / 2.f, World::right);
                Vector4 new_front = rot_mat * normal;
                front = new_front.xyz();
                front.normalize();
                
                is_grounded = true;
                // If we're falling, stop at ground level
                if (velocity.y < 0) {
                    position.y = collision_data.col_point.y;
                    velocity.y = 0.0f;
                }
                break;
            }
            case OBSTACLE: {
                // quit one life
                World::getInstance()->live--;

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


    // this part is to make the jump fluid
    if (jump_time > 0) {
        float jump_force = 1.f;
        jump_time -= dt;
        velocity.y += jump_force * jump_time;
    }


    if (!is_grounded) {
        front[1] = 0.f;
        if (jump_time <= 0) {
            velocity.y += -gravity * dt;
            jump_time = 0;
        }
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