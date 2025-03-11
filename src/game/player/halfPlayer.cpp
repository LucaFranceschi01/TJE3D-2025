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
    model.setTranslation(pos);
    model.scale(0.5f, 0.5f, 0.5f);
}

void HalfPlayer::moveControl(Vector3& move_dir, const float dt)
{
    World* world_instance = World::getInstance();
    if (world_instance->game_mode == World::RELEASE ||
        ((is_left && Input::isKeyPressed(SDL_SCANCODE_W)) ||
            (!is_left && Input::isKeyPressed(SDL_SCANCODE_UP)))) {
        move_dir += front * walk_speed;
        pitch += rotational_speed * dt;
    }
    if (world_instance->game_mode == World::DEBUG &&
        ((is_left && Input::isKeyPressed(SDL_SCANCODE_S)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_DOWN)))) {
        move_dir -= front * walk_speed;
        pitch -= rotational_speed * dt;
    }

    if ((is_left && Input::isKeyPressed(SDL_SCANCODE_A)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_LEFT))) {
        move_dir -= right * 0.5f * walk_speed;
        yaw += rotational_speed * dt * 0.5f;
    }

    if ((is_left && Input::isKeyPressed(SDL_SCANCODE_D)) ||
        (!is_left && Input::isKeyPressed(SDL_SCANCODE_RIGHT))) {
        move_dir += right * 0.5f * walk_speed;
        yaw -= rotational_speed * dt * 0.5f;
    }

    if (Input::gamepads[0].connected) {
        Vector3 left_joystick(Input::gamepads[0].axis[LEFT_ANALOG_Y], 0.f, Input::gamepads[0].axis[LEFT_ANALOG_X]);
        Vector3 right_joystick(Input::gamepads[0].axis[RIGHT_ANALOG_Y], 0.f, Input::gamepads[0].axis[RIGHT_ANALOG_X]);

        if (world_instance->game_mode == World::RELEASE) {
            left_joystick.x = 0.f;
            right_joystick.x = 0.f;
        }

        if (is_left) {
            if (abs(left_joystick.z) > 0.1f) {
                move_dir += right * left_joystick.z * walk_speed;
            }
            if (abs(left_joystick.x) > 0.1f) {
                move_dir -= front * left_joystick.x * walk_speed;
            }
            pitch -= rotational_speed * dt * left_joystick.x;
            yaw -= rotational_speed * dt * left_joystick.z * 0.5f;
        }
        else {
            if (abs(right_joystick.z) > 0.1f) {
                move_dir += right * right_joystick.z * walk_speed;
            }
            if (abs(right_joystick.x) > 0.1f) {
                move_dir -= front * right_joystick.x * walk_speed;
            }
            pitch -= rotational_speed * dt * right_joystick.x;
            yaw -= rotational_speed * dt * right_joystick.z * 0.5f;
        }
    }
}
