//
// Created by Xavi Cañadas on 3/2/25.
//
#include "stage.h"

#include "game/game.h"
#include "game/scene_parser.h"

#include "world.h"


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
}


void PlayStage::render()
{
    //root->render(camera);
    // Create model matrix for cube
    /*
    Matrix44 m;
    m.rotate(angle*DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));

    if (shader)
    {
        // Enable shader
        shader->enable();

        // Upload uniforms
        shader->setUniform("u_color", Vector4(1,1,1,1));
        shader->setUniform("u_viewprojection", camera->viewprojection_matrix );
        shader->setUniform("u_texture", texture, 0);
        shader->setUniform("u_model", m);
        shader->setUniform("u_time", time);

        // Do the draw call
        mesh->render( GL_TRIANGLES );

        // Disable shader
        shader->disable();
    }
    */

    World::getInstance()->render();

    // Draw the floor grid
    drawGrid();
}

void PlayStage::update(float dt)
{
    World::getInstance()->update(dt);
}

void PlayStage::onEnter(Stage* prev_stage)
{
}

void PlayStage::onLeave(Stage* prev_stage)
{
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
    //World::getInstance()->player->onKeyDown(event);

    switch (event.keysym.sym) {
        case SDLK_TAB:
        {
            Game::setMouseLocked();
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
    sColi
}

// está default ahora
MenuStage::MenuStage()
{
}

void MenuStage::init()
{
    Stage::init();
}

void MenuStage::render()
{
    Stage::render();
}

void MenuStage::update(float dt)
{
    Stage::update(dt);
}

void MenuStage::onEnter(Stage* prev_stage)
{
    Stage::onEnter(prev_stage);
}

void MenuStage::onLeave(Stage* prev_stage)
{
}

void MenuStage::onKeyDown(SDL_KeyboardEvent event)
{
    Stage::onKeyDown(event);
}

void MenuStage::onResize(int width, int height)
{
    Stage::onResize(width, height);
}