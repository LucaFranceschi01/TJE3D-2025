//
// Created by Xavi Cañadas on 3/2/25.
//

#pragma once

#include "framework/input.h"

class EntityUI;
class Entity;
class Camera;

typedef enum {
    MAIN_MENU_ST, MAP_SEL_ST, PLAY_ST, DEATH_ST, WIN_ST, UNDEFINED_ST
} StageType;

class Stage
{
public:
    virtual ~Stage() = default; // TODO: does this really matter since virtual ???

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
};

class PlayStage : public Stage
{
public:
    // constructor
    PlayStage();

    // methods
    void init() override;
    void render() override;
    void update(float dt) override;
    void onEnter(Stage* prev_stage) override;
    void onLeave(Stage* prev_stage) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onResize(int width, int height) override;
};

class MenuStage : public Stage
{
public:
    enum e_MenuID {
        MAIN, MAP_SEL, PAUSE, UNDEFINED
    };

    e_MenuID menu;

    Entity* UI_root = nullptr;
    Camera* camera2D = nullptr;

    // constructor
    MenuStage(e_MenuID menu);

    // methods
    void init() override;
    void render() override;
    void update(float dt) override;
    void onEnter(Stage* prev_stage) override;
    void onLeave(Stage* prev_stage) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onResize(int width, int height) override;
};
