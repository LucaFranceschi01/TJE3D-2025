//
// Created by Xavi Cañadas on 3/2/25.
//

#ifndef STAGE_H
#define STAGE_H

#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"



using namespace std; // to avoid writing std:: all the time

typedef enum {
    MENU_STAGE, PLAY_STAGE
} TypeStages;

class Stage {
public:
    virtual ~Stage() = default;

    // methods
    virtual void init() {};
    virtual void render() {};
    virtual void update(double dt) {};

    // events
    virtual void onKeyDown( SDL_KeyboardEvent event ) {};
    virtual void onKeyUp(SDL_KeyboardEvent event) {};
    virtual void onMouseButtonDown( SDL_MouseButtonEvent event ) {};
    virtual void onMouseButtonUp(SDL_MouseButtonEvent event) {};
    virtual void onMouseWheel(SDL_MouseWheelEvent event) {};
    virtual void onGamepadButtonDown(SDL_JoyButtonEvent event) {};
    virtual void onGamepadButtonUp(SDL_JoyButtonEvent event) {};
    virtual void onResize(int width, int height) {};
    virtual void onEnter(Stage* prev_stage) {};
    virtual void onLeave(Stage* new_stage){};

};

class PlayStage : public Stage {
public:
    // constructor
    PlayStage();

    // methods
    void init() override;
    void render() override;
    void update(double dt) override;

    // Esta func es útil para resetear cosas, por ejemplo pasar del menu al play
    void onEnter(Stage *prev_stage) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onResize(int width, int height) override;

};

class MenuStage : public Stage {
public:
    // constructor
    MenuStage();

    // methods
    void init() override;
    void render() override;
    void update(double dt) override;

    // Esta func es útil para resetear cosas, por ejemplo pasar del menu al play
    void onEnter(Stage *prev_stage) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onResize(int width, int height) override;
};



#endif //STAGE_H
