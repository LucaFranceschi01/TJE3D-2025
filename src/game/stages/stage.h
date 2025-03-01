/*  by Xavier Cañadas and Luca Franceschi 2025
    Here we define the base Stage class. It should not have any cpp file associated since it only defines signatures
*/

#pragma once

#include "SDL2/SDL_events.h"

class Entity;
class Camera;

typedef enum {
    MAIN_MENU_ST, MAP_SEL_ST, PLAY_ST, DEATH_ST, WIN_ST, UNDEFINED_ST
} StageType;

class Stage
{
public:
    StageType stage_type = UNDEFINED_ST;

    virtual ~Stage() = default;

    // methods
    virtual void init() {};
    virtual void render() {};
    virtual void update(float dt) {};

    // events
    virtual void onKeyDown(SDL_KeyboardEvent event) {};
    virtual void onKeyUp(SDL_KeyboardEvent event) {};
    virtual void onMouseButtonDown(SDL_MouseButtonEvent event) {};
    virtual void onMouseButtonUp(SDL_MouseButtonEvent event) {};
    virtual void onMouseWheel(SDL_MouseWheelEvent event) {};
    virtual void onGamepadButtonDown(SDL_JoyButtonEvent event) {};
    virtual void onGamepadButtonUp(SDL_JoyButtonEvent event) {};
    virtual void onResize(int width, int height) {};
    virtual void onEnter(Stage* prev_stage) {};
    virtual void onLeave(Stage* new_stage) {};

    virtual void switchPauseResume() {};
    virtual void removeLifeUI() {};
};
