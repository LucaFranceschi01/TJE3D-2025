#include "menuStage.h"

#include "game/world.h"
#include "game/game.h"

#include "graphics/shader.h"

#include "framework/camera.h"
#include "framework/entities/entityUI.h"
#include "framework/input.h"

#include <algorithm>

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

    Entity* UI_root_tmp = new Entity();

    Material mat;
    mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

    switch (menu)
    {
    case MenuStage::MAIN:
    {   
        stage_type = MAIN_MENU_ST;
        UI_root = UI_root_tmp;

        EntityUI* btn = new EntityUI(mat, Vector2(width/2.f, height/2.f),
            Vector2(width, height), "data/textures/ui/background.png");
        UI_root->addChild(btn);

        btn = new EntityUI(mat, Vector2(width / 3.f, 500.f),
            btn_size, "play", EntityUI::ENTER_MAP_SELECTOR);
        UI_root->addChild(btn);

        btn = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f),
            btn_size, "exit", EntityUI::EXIT);
        UI_root->addChild(btn);

        break;
    }
    case MenuStage::MAP_SEL:
    {
        stage_type = MAP_SEL_ST; 
        UI_root = UI_root_tmp;
        
        mapNames = { "M1_Tilted_Tenpins", "M2_Twisted_Lanes" , "M3_Tenpin_Crusher" };

        EntityUI* btn = new EntityUI(mat, Vector2(width / 2.f, height / 2.f),
            Vector2(width, height), "data/textures/ui/background.png");
        UI_root->addChild(btn);
        
        Material mat_flat;
        mat_flat.color = { 0.f, 0.f, 0.f, 0.4 };

        btn = new EntityUI(mat_flat, Vector2(width / 2.f, height / 2.f),
            Vector2(width, height));
        UI_root->addChild(btn);

        uint8 mapID = 0;
        for (std::string name : mapNames) {
            btn = new EntityUI(mat, Vector2(width / 2.f + mapID, height / 2.f),
                Vector2(256.f, 256.f), std::string("data/textures/ui/") + name + std::string(".png"), EntityUI::MAP_THUMBNAIL);
            btn->mapID = mapID;
            UI_root->addChild(btn);
            mapID++;
        }
        
        btn = new EntityUI(mat, Vector2(width / 6.f, height / 2.f),
            btn_size, "left", EntityUI::PREVIOUS_MAP);
        UI_root->addChild(btn);

        btn = new EntityUI(mat, Vector2(5.f * width / 6.f, height / 2.f),
            btn_size, "right", EntityUI::NEXT_MAP);
        UI_root->addChild(btn);

        btn = new EntityUI(mat, Vector2(width / 3.f, 500.f),
            btn_size, "start", EntityUI::START_MAP);
        UI_root->addChild(btn);

        btn = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f),
            btn_size, "back", EntityUI::EXIT);
        UI_root->addChild(btn);

        break;
    }
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

    switch (menu) {
    case MenuStage::MAP_SEL:
    {
        Game* instance = Game::instance;
        std::string name = mapNames[instance->currentMap];
        std::replace(name.begin(), name.end(), '_', ' ');
        std::vector<float> text_length = { 92.f, 91.f, 96.f }; // hardcoded by looking at lengths of text below to center them
        float scaling = 5.f;
        drawText(instance->window_width / 2.f - text_length[instance->currentMap] * scaling / 2.f,
            instance->window_height * 1.f / 5.f, name, Vector3(1.f), scaling);


        scaling = 4.f;
        float len = 28.f + 15.f + 5.f * 2;
        if (instance->total_pins == 0) {
            len += 5.f;
        }
        else {
            len += ceil(log10(instance->total_pins) + 0.000000001f) * 5.f;
        }
        drawText(instance->window_width / 2.f - len * scaling / 2.f, instance->window_height * 10.f / 11.f,
            std::string("Pins: ") + std::to_string(instance->total_pins) + std::string(" / ") + std::to_string(30),
            Vector3(1.f), scaling);
        break;
    }
    }
}

void MenuStage::update(float dt)
{
    UI_root->update(dt);
}

void MenuStage::onKeyDown(SDL_KeyboardEvent event)
{
    Game* instance = Game::instance;
    switch (event.keysym.sym) {
    case SDLK_RIGHT:
        instance->nextMap();
        break;
    case SDLK_LEFT:
        instance->previousMap();
        break;
    case SDLK_RETURN:
        if (instance->currentStage->stage_type == MAIN_MENU_ST) {
            instance->goToStage(MAP_SEL_ST);
        }
        else if (instance->currentStage->stage_type == MAP_SEL_ST) {
            instance->goToStage(PLAY_ST);
        }
        break;
    }
}