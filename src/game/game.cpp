#include "game.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"

#include "framework/input.h"
#include "framework/audio.h"
#include "framework/camera.h"
#include "framework/entities/entityMesh.h"

#include "game/scene_parser.h"
#include "game/world.h"

#include "stages/menuStage.h"
#include "stages/playStage.h"

#include <cmath>

Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
    this->window_width = window_width;
    this->window_height = window_height;
    this->window = window;
    instance = this;
    must_exit = false;

    fps = 0;
    frame = 0;
    time = 0.0f;
    elapsed_time = 0.0f;
    mouse_locked = false;

    Audio::Init();

    // construct and init all stages and set the first one
    stages[MAIN_MENU_ST] = new MenuStage(MAIN_MENU_ST);
    stages[INTRO_ST] = new MenuStage(INTRO_ST);
    stages[MAP_SEL_ST] = new MenuStage(MAP_SEL_ST);
    stages[PLAY_ST] = new PlayStage();
    stages[DEATH_ST] = new MenuStage(DEATH_ST);
    stages[WIN_ST] = new MenuStage(WIN_ST);
    stages[OUTRO_ST] = new MenuStage(OUTRO_ST);

    for (auto& entry : stages) {
        Stage* stage = entry.second;
        stage->init();
    }

    goToStage(MAIN_MENU_ST);

    // OpenGL flags
    glEnable(GL_CULL_FACE); // do not render both sides of every triangle
    glEnable(GL_DEPTH_TEST); // check the occlusions using the Z buffer

    // Hide the cursor
    SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{
    // Set the clear color (the background color)
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set flags
    glEnable(GL_CULL_FACE); // do not render both sides of every triangle
    glEnable(GL_DEPTH_TEST); // check the occlusions using the Z buffer

    if (currentStage != nullptr) {
        currentStage->render();
    }

    // Render the FPS, Draw Calls, etc
    drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

    // Swap between front buffer and back buffer
    SDL_GL_SwapWindow(this->window);
}

void Game::update(double dt)
{
    if (currentStage != nullptr) {
        currentStage->update(static_cast<float>(dt));
    }
}

void Game::goToStage(StageType stage)
{
    Stage* new_stage = stages[stage];
    assert(new_stage);

    if (currentStage != nullptr) {
        currentStage->onLeave(new_stage);
    }

    new_stage->onEnter(currentStage);

    currentStage = new_stage;
}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym) {
        case SDLK_ESCAPE: must_exit = true;
            break; //ESC key, kill the app
        case SDLK_F1: Shader::ReloadAll();
            break;
    }

    if (currentStage != nullptr) {
        currentStage->onKeyDown(event);
    }
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
    if (currentStage != nullptr) {
        currentStage->onKeyUp(event);
    }
}

void Game::onMouseButtonDown(SDL_MouseButtonEvent event)
{
    // middle mouse
    if (event.button == SDL_BUTTON_MIDDLE) {
        mouse_locked = !mouse_locked;
        SDL_ShowCursor(!mouse_locked);
        SDL_SetRelativeMouseMode((SDL_bool)(mouse_locked));
    }

    if (currentStage != nullptr)
        currentStage->onMouseButtonDown(event);
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (currentStage != nullptr)
        currentStage->onMouseButtonUp(event);
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
    mouse_speed *= event.y > 0 ? 1.1f : 0.9f;

    if (currentStage != nullptr)
        currentStage->onMouseWheel(event);
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
    if (currentStage != nullptr)
        currentStage->onGamepadButtonDown(event);
}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{
    if (currentStage != nullptr)
        currentStage->onGamepadButtonUp(event);
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
    glViewport(0, 0, width, height);
    window_width = width;
    window_height = height;

    if (currentStage != nullptr)
        currentStage->onResize(width, height);
}

void Game::setMouseLocked()
{
    bool must_lock = false;

    World::e_camera_mode* camera_mode = &World::getInstance()->camera_mode;
    *camera_mode = World::e_camera_mode((*camera_mode + 1) % World::NR_CAMERA_MODES);
    
    if (*camera_mode == World::FIXED) {
        must_lock = true;
    }
    
    Game::instance->mouse_locked = must_lock;

    SDL_ShowCursor(!must_lock);

	SDL_SetRelativeMouseMode((SDL_bool)must_lock);

	mouse_locked = must_lock;
}

void Game::nextMap()
{
    if (currentMap < MAP_COUNT - 1) {
        currentMap++;
    }
}

void Game::previousMap()
{
    if (currentMap > 0) {
        currentMap--;
    }
}