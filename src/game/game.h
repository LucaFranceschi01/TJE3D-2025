/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#pragma once

#include "framework/utils.h"

#include "game/stages/stage.h"

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
    std::map<StageType, Stage*> stages;
    uint8 currentMap = 0;

    float angle = 0;
    float mouse_speed = 50.0f;
    bool debug_view = false;

    Game(int window_width, int window_height, SDL_Window* window);

    //main functions
	void render( void );
	void update( double dt );

	void setMouseLocked();

	//events
    void goToStage(StageType stage);
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

    void nextMap();
    void previousMap();
};
