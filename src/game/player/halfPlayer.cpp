#include "halfPlayer.h"

#include "framework/input.h"

#include "game/world.h"

HalfPlayer::HalfPlayer(Mesh* mesh, const Material& material, bool is_left, const std::string& name)
    : Player(mesh, material, name)
{
    this->is_left = is_left;
}

void HalfPlayer::init(const Vector3& pos)
{
    Vector3 new_pos = pos;
    new_pos.z += (is_left) ? -5 : 5;
    model.setTranslation(new_pos);
    model.scale(0.5f, 0.5f, 0.5f);
}

void HalfPlayer::moveControl(Vector3& move_dir, const float dt)
{
    World* world_instance = World::getInstance();
    if (world_instance->game_mode == World::RELEASE ||
        ((is_left && Input::isKeyPressed(SDL_SCANCODE_W)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_UP)))) {

        move_dir += front * dt;
        pitch += rotational_speed * dt;
    }
    if ((is_left && Input::isKeyPressed(SDL_SCANCODE_S)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_DOWN))) {

        move_dir -= front * dt;
        pitch -= rotational_speed * dt;
    }

    if ((is_left && Input::isKeyPressed(SDL_SCANCODE_A)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_LEFT))) {
        move_dir -= right * dt;
        yaw += rotational_speed * dt;
    }

    if ((is_left && Input::isKeyPressed(SDL_SCANCODE_D)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_RIGHT))) {
        move_dir += right * dt;
        yaw -= rotational_speed * dt;
    }
}
