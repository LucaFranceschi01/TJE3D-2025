/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#pragma once

#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "game/stage.h"

class Game
{
public:
    static Game* instance;

    //window
    SDL_Window* window;
    int window_width;
    int window_height;

    //some globals
    long frame;
    float time;
    float elapsed_time;
    int fps;
    bool must_exit;

    //some vars
    bool mouse_locked; //tells if the mouse is locked (not seen)
    Stage* currentStage = nullptr;
    std::map<TypeStages, Stage*> stages;

    float angle = 0;
    float mouse_speed = 100.0f;


    Game(int window_width, int window_height, SDL_Window* window);

    //main functions
    void render(void);

    void update(double dt);

    // events
    void setMouseLocked(bool mouse_lock);

    void goToStage(TypeStages type_stage);

    void onKeyDown(SDL_KeyboardEvent event);

    void onKeyUp(SDL_KeyboardEvent event);

    void onMouseButtonDown(SDL_MouseButtonEvent event);

    void onMouseButtonUp(SDL_MouseButtonEvent event);

    void onMouseWheel(SDL_MouseWheelEvent event);

    void onGamepadButtonDown(SDL_JoyButtonEvent event);

    void onGamepadButtonUp(SDL_JoyButtonEvent event);

    void onResize(int width, int height);
};
