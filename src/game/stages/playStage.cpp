//
// Created by Xavi Cañadas on 22/2/25.
//

#include "playStage.h"

#include "game/player/player.h"
#include "game/world.h"
#include "game/game.h"

PlayStage::PlayStage()
{
    /*
     entity_mesh = new EntityMesh(mesh, material);
     SceneParser parser;
     parser.parse("data/myscene.scene", root); // TODO: in blender do @player tag parser thing
     */
}

void PlayStage::init()
{
    World::getInstance()->init(World::TESTING);
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
    World::getInstance()->init(World::MAP1);
    World::getInstance()->player->init();

}

void PlayStage::onLeave(Stage* prev_stage)
{
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