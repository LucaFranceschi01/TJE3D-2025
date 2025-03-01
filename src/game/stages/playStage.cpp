#include "playStage.h"

#include "game/player/player.h"
#include "game/world.h"
#include "game/game.h"

#include "framework/camera.h"
#include "graphics/shader.h"
#include "framework/entities/entityUI.h"

PlayStage::PlayStage()
{
    World* instance = World::getInstance();

    camera2D = new Camera();
    camera2D->view_matrix = Matrix44(); // Set View to identity, maybe not needed
    camera2D->setOrthographic(0.f, instance->window_width, instance->window_height, 0.f, -1.f, 1.f);

    stage_type = PLAY_ST;

    float width = instance->window_width;
    float height = instance->window_height;
    Vector2 wide_btn_size(128.f, 64.f);
    Vector2 square_btn_size(32.f, 32.f);

    UI_root = new Entity();

    Material mat;
    mat.color = { 0.f, 0.f, 0.f, 0.4 };

    EntityUI* btn = new EntityUI(mat, Vector2(width / 2.f, height / 2.f),
        Vector2(width, height));
    btn->visible = false;
    UI_root->addChild(btn);

    mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

    btn = new EntityUI(mat, Vector2(64.f, 64.f),
        square_btn_size, "pause", EntityUI::PAUSE);
    UI_root->addChild(btn);

    btn = new EntityUI(mat, Vector2(width / 2.f, 2.f * height / 5.f),
        wide_btn_size, "resume", EntityUI::RESUME);
    btn->visible = false;
    UI_root->addChild(btn);

    btn = new EntityUI(mat, Vector2(width / 2.f, 3.f * height / 5.f),
        wide_btn_size, "back2", EntityUI::ENTER_MAP_SELECTOR);
    btn->visible = false;
    UI_root->addChild(btn);
}

void PlayStage::init()
{
}

void PlayStage::render()
{
    World::getInstance()->render();

    UI_root->render(camera2D);
}

void PlayStage::update(float dt)
{
    if (!Game::instance->paused) {
        World::getInstance()->update(dt);
    }

    UI_root->update(dt);
}

void PlayStage::onEnter(Stage* prev_stage)
{
    World::getInstance()->init(); // TODO: CHANGE SIGNATURE SO THAT IT MEANS ENTER A MAP
    if (Game::instance->paused) {
        switchPauseResume();
    }
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

void PlayStage::switchPauseResume()
{
    Game::instance->paused = !Game::instance->paused;
    for (Entity* element : UI_root->children) {
        EntityUI* ui_element = static_cast<EntityUI*>(element);
        // TODO: poner bonito al acabar
        if (ui_element->buttonID == EntityUI::PAUSE) ui_element->visible = !ui_element->visible;
        if (ui_element->buttonID == EntityUI::RESUME) ui_element->visible = !ui_element->visible;
        if (ui_element->buttonID == EntityUI::UNDEFINED) ui_element->visible = !ui_element->visible;
        if (ui_element->buttonID == EntityUI::ENTER_MAP_SELECTOR) ui_element->visible = !ui_element->visible;
    }
}