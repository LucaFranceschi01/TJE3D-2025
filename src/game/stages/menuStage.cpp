//
// Created by Xavi Cañadas on 22/2/25.
//

#include "menuStage.h"

#include "game/world.h"
#include "game/game.h"
#include "game/scene_parser.h"

#include "graphics/shader.h"

#include "framework/camera.h"
#include "framework/entities/entityUI.h"

MenuStage::MenuStage(e_MenuID menu)
{
    World* instance = World::getInstance();

    camera2D = new Camera();
    //camera2D->lookAt(Vector3(0.f), Vector3(0.f, 0.f, -1.f), Vector3::UP);
    camera2D->view_matrix = Matrix44(); // Set View to identity
    camera2D->setOrthographic(0.f, instance->window_width, instance->window_height, 0.f, -1.f, 1.f);

    this->menu = menu;
}

void MenuStage::init()
{
    World* instance = World::getInstance();
    float width = instance->window_width;
    float height = instance->window_height;
    Vector2 btn_size(128.f, 64.f);

    switch (menu)
    {
    case MenuStage::MAIN:
    {
        UI_root = new Entity();

        Material mat;
        mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
        
        EntityUI* btn = new EntityUI(mat, Vector2(width/2.f, height/2.f),
            Vector2(width, height), std::string("data/textures/ui/background.png"));
        UI_root->addChild(btn);

        btn = new EntityUI(mat, Vector2(width / 3.f, 500.f),
            btn_size, "play", EntityUI::e_UIButtonID::START_MAP);
        UI_root->addChild(btn);

        btn = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f), btn_size,
            "exit", EntityUI::e_UIButtonID::EXIT);
        UI_root->addChild(btn);

        break;
    }
    case MenuStage::MAP_SEL:
        break;
    case MenuStage::PAUSE:
        break;
    case MenuStage::UNDEFINED:
        break;
    default:
        break;
    }

    Stage::init();
}

void MenuStage::render()
{
    UI_root->render(camera2D);
    Stage::render();
}

void MenuStage::update(float dt)
{
    UI_root->update(dt);
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