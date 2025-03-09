#include "player.h"

#include "game/game.h"
#include "game/world.h"
#include "game/stages/stage.h"

#include "framework/audio.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "framework/entities/entityUI.h"
#include "game/stages/playStage.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"


Player::Player(Mesh* mesh, const Material& material, const std::string& name) : EntityMesh(mesh, material, name)
{
    Game* game_instance = Game::instance;
    PlayStage* play_stage = dynamic_cast<PlayStage*>(game_instance->stages[PLAY_ST]);
    if (play_stage == nullptr) return;

    float width = game_instance->window_width;

    Material mat;
    mat.color = { 0.f, 0.f, 0.f, 0.0f };

}


void Player::init(const Vector3& pos)
{
    World::getInstance()->live = 3;
    model.setTranslation(pos);
    yaw = 0.f;
    pitch = 0.f;
    velocity = Vector3(0.f);
    jump_time = 0.0f;
    front = Vector3(0.f);
    right = Vector3(0.0f);
    collision = false;
    collision_fluid = false;
    time_colision_fluid = 0;
    time_fluid_i = 0;
    fluid_factor = 0;
    booster = NONE_BOOSTER;

    PlayStage* play_stage = dynamic_cast<PlayStage*>(Game::instance->stages[PLAY_ST]);
    if (play_stage == nullptr) return;

}

void Player::render(Camera* camera)
{
    if (Game::instance->debug_view) {
        std::string debug_str = "normal_orig: " + normal_orig.to_string();
        drawText(500, 16, debug_str, Vector3(1, 1, 1), 2);

        debug_str = "front: " + front.to_string();
        drawText(550, 16 * 2, debug_str, Vector3(1, 1, 1), 2);

        debug_str = "velocity: " + velocity.to_string();
        drawText(500, 16 * 6, debug_str, Vector3(1, 1, 1), 2);

        debug_str = "move_dir: " + move_dir.to_string();
        drawText(500, 16 * 7, debug_str, Vector3(1, 1, 1), 2);

        debug_str = "position: " + model.getTranslation().to_string();
        drawText(500, 16 * 8, debug_str, Vector3(1, 1, 1), 2);

        debug_str = "fluid factor: " + std::to_string(fluid_factor);
        drawText(500, 16 * 9, debug_str, Vector3(1, 1, 1), 2);

        debug_str = "booster type: " + std::to_string(booster);
        drawText(500, 16 * 10, debug_str, Vector3(1, 1, 1), 2);

        debug_str = "booster time: " + std::to_string(time_booster);
        drawText(500, 16 * 11, debug_str, Vector3(1, 1, 1), 2);

        renderDebug(camera);
    }

    EntityMesh::render(camera);
}

void Player::update(float dt)
{
    World* world_instance = World::getInstance();
    Vector3 position = model.getTranslation();

    // Death cutscene
    if (world_instance->live <= 0 || position.y <= 0) {
        Audio::Play("data/sounds/sad_horn.wav");
        Game::instance->goToStage(DEATH_ST);
    }

    // If camera is in free mode, avoid moving the player
    if (world_instance->camera_mode == World::e_camera_mode::FREE) return;

    // Set moving directions
    front = World::front;
    right = World::right;

    // Test collisions
    bool is_colliding = testCollisions(position, dt);

    move_dir = Vector3(0.0f);

    moveControl(move_dir, dt);

    float speed_mult = walk_speed;

    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
        speed_mult *= 0.3f;

    // TODO: decide
    //move_dir.normalize(); If we do not normalize, the half player will be always be parallel.
    // I dont know if its ok to no normalize

    //velocity += 1000 * (move_dir * speed_mult) * dt;

    if (collision_fluid) {
        
        time_colision_fluid += dt;
        if (time_colision_fluid - time_fluid_i > 0.25) {
            fluid_factor = rand() % 5 - 2;
            time_fluid_i = time_colision_fluid;
        }
        velocity.z += 25.0f * static_cast<float>(fluid_factor) * dt;

        velocity += 0.12 * 1000.f * (move_dir * speed_mult) * dt;
    }
    else {
        velocity += 1000 * (move_dir * speed_mult) * dt;
    }

    // Update player position. Colliding with wall.
    position += (is_colliding) ? velocity * dt : -velocity * 10.f * dt;

    // Get the rotation of the ball
    yaw = positive_modulo(yaw, 360.f*DEG2RAD);
    pitch = positive_modulo(pitch, 360.f*DEG2RAD);

    // Rotate the ball according to current pitch and yaw
    model.setTranslation(position);
    model.rotate(yaw, World::front);
    model.rotate(pitch, World::right);

    // Decrease velocities and rotations when not moving
    if (!collision_fluid) {
        velocity.x *= 0.9f;
        velocity.z *= 0.9f;

        if (move_dir.z == 0.f) {
            dampen(&yaw);
        }
        if (move_dir.x == 0.f) {
            dampen(&pitch);
        }
    }
    else {
        velocity.x = clamp(velocity.x, -45.f, 45.f);
        velocity.z = clamp(velocity.z, -23.f, 23.f);
    }

    // booster update

    if (time_choosing_booster > 0) {
        time_choosing_booster -= dt;
        if (time_choosing_booster <= 0) {
            choosing_booster = false;
            time_choosing_booster = 0;
        }
    } else {
        if (time_booster > 0) {
            time_booster -= dt;
            if (time_booster <= 0) {
                time_booster = 0;
                booster = NONE_BOOSTER;
            }
        }
    }



    EntityMesh::update(dt);
}

void Player::moveControl(Vector3& move_dir, const float dt)
{
    World* world_instance = World::getInstance();
    if (world_instance->game_mode == World::RELEASE ||
        (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP))) {
        move_dir += front;
        pitch += rotational_speed * dt;
    }
    // if DEBUG is not here, the user in release can stop the player. move_dir + front - front = 0
    if (world_instance->game_mode == World::DEBUG &&
        (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN))) {
        move_dir -= front;
        pitch -= rotational_speed * dt;
    }
    if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
        if (Input::isKeyPressed(SDL_SCANCODE_Z)) {
            if (World::front.z < 0) {
                World::front.x -= dt;
                World::front.z -= dt;

                World::right.x += dt;
                World::right.z -= dt;

                World::front.normalize();
                World::right.normalize();
            } else {
                World::front.x +=  dt;
                World::front.z -= dt;

                World::right.x += dt;
                World::right.z += dt;

                World::front.normalize();
                World::right.normalize();
            }
        } else {
            move_dir -= right * 0.5f;
            yaw += rotational_speed * dt * 0.5f;
        }

    }
    if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
        if (Input::isKeyPressed(SDL_SCANCODE_Z)) {
            if (World::front.z < 0) {
                World::front.x +=  dt;
                World::front.z += dt;

                World::right.x -= dt;
                World::right.z += dt;

                World::front.normalize();
                World::right.normalize();
            } else {
                World::front.x -=  dt;
                World::front.z += dt;

                World::right.x -= dt;
                World::right.z -= dt;

                World::front.normalize();
                World::right.normalize();
            }
        } else {
            move_dir += right * 0.5f;
            yaw -= rotational_speed * dt * 0.5f;
        }
    }

}

bool Player::testCollisions(const Vector3& position, float dt)
{
    // TODO: PUT EVERYTHING INTO GAME SO WE CAN DO ONLY GAME::INSTANCE INSTEAD OF ALSO WORLD
    
    // Check collisions with the world entities
    std::vector<sCollisionData> collisions;
    std::vector<sCollisionData> ground_collisions;
    World* instance = World::getInstance();

    // calls test_scene_collision to check if the new position collides with something.
    instance->test_scene_collisions(position + velocity * dt, collisions, ground_collisions);

    bool is_grounded = false;
    collision_fluid = false;
    Game* game_instance = Game::instance;

    // environment collisions
    for (const sCollisionData& collision_data : collisions) {
        switch (collision_data.collider->collision_type) {
            case GROUND:
            {
                normal_orig = collision_data.col_normal;

                Vector4 normal = Vector4(normal_orig.normalize(), 0.f);
                float res = normal_orig.dot(Vector3::UP);
                if (res < 0.1) {
                    return false;
                }

                Matrix44 rot_mat;
                rot_mat.setRotation(PI / 2.f, World::right);
                Vector4 new_front = rot_mat * normal;
                front = new_front.xyz();
                front.normalize();

                is_grounded = true;
                // If we're falling, stop at ground level
                velocity.y = std::max(0.f, velocity.y);
                break;
            }
            case OBSTACLE: {
                // quit one life
                if (booster != INMORTAL) {
                    instance->live--;
                    game_instance->currentStage->removeLifeUI();
                }

                // send the object to delete
                instance->destroyEntity(collision_data.collider, collision_data.col_point);
                break;
            }
            case FLUID: {
                if (booster != SPIKES) {
                    collision_fluid = true;
                }
                break;
            }
            case PIN: {
                game_instance->currentStage->collectPin();

                // send the object to delete
                instance->destroyEntity(collision_data.collider, collision_data.col_point);
                break;
            }
            case BOOSTER: {
                int range_booster = (instance->live < 3) ? 3 : 2; // no not allow more than three lives

                booster = static_cast<eBooster>(rand() % range_booster); // booster 0, 1 or 2
                time_booster = 10;

                instance->destroyEntity(collision_data.collider, collision_data.col_point);

                if (booster == EXTRA_LIVE) {
                    instance->live++;
                }
                choosing_booster = true;
                time_choosing_booster = 3;
                break;
            }
            case FINISHLINE:
            {
                game_instance->goToStage(WIN_ST); // TODO: CREATE WIN STAGE
                break;
            }
            default:
                break;
        }
    }

    // this part is to make the jump fluid
    if (jump_time > 0) {
        float jump_force = 1.f;
        jump_time -= dt;
        velocity.y += jump_force * jump_time;
    }

    if (!is_grounded) {
        front[1] = 0.f;
        if (jump_time <= 0) {
            velocity.y += -100.0f * dt;
            jump_time = 0;
        }
    }
    // Handle jumping with better feel
    else if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
        jump_time = 0.25f;
    }

    collision = (!collisions.empty());

    if (!collision_fluid) {
        fluid_factor = 0;
    }

    return true;
}

void Player::renderDebug(Camera* camera)
{
    Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
    Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");
    Matrix44 m = model;

    shader->enable();

    {
        float sphere_radius = 1.f;
        Vector4 color;

        if (collision_fluid) {
            color = Vector4(0.0f, 1.0f, 1.0f, 1.0f);

        } else if (collision) {
            color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
        } else {
            color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        m.scale(sphere_radius, sphere_radius, sphere_radius);
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
