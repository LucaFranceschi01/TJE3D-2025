#include "menuStage.h"

#include "game/world.h"

#include "graphics/shader.h"

#include "framework/camera.h"
#include "framework/entities/entityUI.h"

MenuStage::MenuStage(e_MenuID menu)
{
    World* instance = World::getInstance();

    camera2D = new Camera();
    camera2D->view_matrix = Matrix44(); // Set View to identity, maybe not needed
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

        btn = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f),
            btn_size, "exit", EntityUI::e_UIButtonID::EXIT);
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
}

void MenuStage::render()
{
    UI_root->render(camera2D);
}

void MenuStage::update(float dt)
{
    UI_root->update(dt);
}
