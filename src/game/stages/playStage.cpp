#include "playStage.h"

#include "game/player/player.h"
#include "game/world.h"
#include "game/game.h"

#include "framework/camera.h"

void PlayStage::init()
{
    stage_type = PLAY_ST;
}

void PlayStage::render()
{
    World::getInstance()->render();
}

void PlayStage::update(float dt)
{
    World::getInstance()->update(dt);
}

void PlayStage::onEnter(Stage* prev_stage)
{
    World::getInstance()->init(); // TODO: CHANGE SIGNATURE SO THAT IT MEANS ENTER A MAP
    // TODO: SET UI VISIBILITY
}

void PlayStage::onLeave(Stage* prev_stage)
{
    // TODO: SET UI VISIBILITY
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
    World::getInstance()->onKeyDown(event);

    switch (event.keysym.sym) {
        case SDLK_TAB:
        {
            Game::instance->setMouseLocked();
            break;
        }
        case SDLK_RSHIFT:
        {
            Game::instance->debug_view = !Game::instance->debug_view;
            break;
        }
    }
}

void PlayStage::onResize(int width, int height)
{
    World* world = World::getInstance();
    world->camera->aspect = static_cast<float>(width) / static_cast<float>(height);
}
