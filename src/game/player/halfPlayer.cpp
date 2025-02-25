//
// Created by Xavi Cañadas on 20/2/25.
//

#include "halfPlayer.h"

#include "framework/audio.h"
#include "game/game.h"
#include "game/world.h"


void HalfPlayer::init(Vector3 pos)
{
    pos.z += (is_left) ? -5 : 5;
    model.setTranslation(pos);
    model.scale(0.5f, 0.5f, 0.5f);
}


void HalfPlayer::moveControl(Vector3& move_dir, float dt)
{
    World* world_instance = World::getInstance();
    if (world_instance->game_mode == World::RELEASE ||
        ((is_left && Input::isKeyPressed(SDL_SCANCODE_W)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_UP)))) {

        move_dir += front;
        pitch += rotational_speed * dt;
    }
    if (world_instance->game_mode == World::DEBUG  &&
        ((is_left && Input::isKeyPressed(SDL_SCANCODE_S)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_DOWN)))) {

        move_dir -= front;
        pitch -= rotational_speed * dt;
    }

    if ((is_left && Input::isKeyPressed(SDL_SCANCODE_A)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_LEFT))) {
        move_dir -= right;
        yaw += rotational_speed * dt;
    }

    if ((is_left && Input::isKeyPressed(SDL_SCANCODE_D)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_RIGHT))) {
        move_dir += right;
        yaw -= rotational_speed * dt;
    }
}
