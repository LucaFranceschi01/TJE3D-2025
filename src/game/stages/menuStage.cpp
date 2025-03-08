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

    UI_root = new Entity();

    EntityUI* ui_elem;
    Material mat;
    Material mat_flat;
    mat_flat.color = { 0.f, 0.f, 0.f, 0.4 };
    mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

    ui_elem = new EntityUI(mat, Vector2(width / 2.f, height / 2.f),
        Vector2(width, height), "data/textures/ui/background.png");
    UI_root->addChild(ui_elem);

    switch (menu)
    {
    case MenuStage::MAIN:
    {   
        stage_type = MAIN_MENU_ST;

        ui_elem = new EntityUI(mat, Vector2(width / 3.f, 500.f),
            btn_size, "play", EntityUI::ENTER_MAP_SELECTOR);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f),
            btn_size, "exit", EntityUI::EXIT);
        UI_root->addChild(ui_elem);

        break;
    }
    case MenuStage::MAP_SEL:
    {
        stage_type = MAP_SEL_ST; 
        
        mapNames = { "M1_Tilted_Tenpins", "M2_Twisted_Lanes" , "M3_Tenpin_Crusher" };

        ui_elem = new EntityUI(mat_flat, Vector2(width / 2.f, height / 2.f),
            Vector2(width, height));
        UI_root->addChild(ui_elem);

        uint8 mapID = 0;
        for (std::string name : mapNames) {
            ui_elem = new EntityUI(mat, Vector2(width / 2.f + mapID, height / 2.f),
                Vector2(256.f, 256.f), std::string("data/textures/ui/") + name + std::string(".png"), EntityUI::MAP_THUMBNAIL);
            ui_elem->mapID = mapID;
            UI_root->addChild(ui_elem);
            mapID++;
        }
        
        ui_elem = new EntityUI(mat, Vector2(width / 6.f, height / 2.f),
            btn_size, "left", EntityUI::PREVIOUS_MAP);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(5.f * width / 6.f, height / 2.f),
            btn_size, "right", EntityUI::NEXT_MAP);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width / 3.f, 500.f),
            btn_size, "start", EntityUI::START_MAP);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f),
            btn_size, "back", EntityUI::EXIT);
        UI_root->addChild(ui_elem);

        break;
    }
    case MenuStage::PAUSE:
        break;
    case MenuStage::DEATH:
    {
        stage_type = DEATH_ST;

        ui_elem = new EntityUI(mat_flat, Vector2(width / 2.f, height / 2.f),
            Vector2(width, height));
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width / 3.f, 500.f),
            btn_size, "yes", EntityUI::START_MAP);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f),
            btn_size, "no", EntityUI::ENTER_MAP_SELECTOR);
        UI_root->addChild(ui_elem);
        break;
    }
    case MenuStage::WIN:
    {
        stage_type = WIN_ST;

        ui_elem = new EntityUI(mat_flat, Vector2(width / 2.f, height / 2.f),
            Vector2(width, height));
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width / 3.f, 500.f),
            btn_size, "yes", EntityUI::NEXT_MAP);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f),
            btn_size, "no", EntityUI::ENTER_MAP_SELECTOR);
        UI_root->addChild(ui_elem);
        break;
    }
    case MenuStage::UNDEFINED:
        break;
    default:
        break;
    }
}

void MenuStage::render()
{
    UI_root->render(camera2D);

    Game* instance = Game::instance;
    float scaling, len;

    switch (menu) {
    case MenuStage::MAP_SEL:
    {
        std::string name = mapNames[instance->currentMap];
        std::replace(name.begin(), name.end(), '_', ' ');
        std::vector<float> text_length = { 92.f, 91.f, 96.f }; // hardcoded by looking at lengths of text below to center them
        scaling = 5.f;
        drawText(instance->window_width / 2.f - text_length[instance->currentMap] * scaling / 2.f,
            instance->window_height * 1.f / 5.f, name, Vector3(1.f), scaling);

        scaling = 4.f;
        len = 28.f + 15.f + 5.f * 2;
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
    case MenuStage::DEATH:
    {
        scaling = 10.f;
        len = 44.f;
        drawText(instance->window_width / 2.f - len * scaling / 2.f, instance->window_height * 1.f / 3.f,
            std::string("You lose"), Vector3(1.f), scaling);

        scaling = 5.f;
        len = 66.f;
        drawText(instance->window_width / 2.f - len * scaling / 2.f, instance->window_height * 2.f / 3.f,
            std::string("Repeat map?"), Vector3(1.f), scaling);
        break;
    }
    case MenuStage::WIN:
    {
        scaling = 10.f;
        len = 40.f;
        drawText(instance->window_width / 2.f - len * scaling / 2.f, instance->window_height * 1.f / 3.f,
            std::string("You win"), Vector3(1.f), scaling);


        scaling = 5.f;
        if (instance->currentMap != MAP_COUNT - 1) {
            len = 89.f;
            drawText(instance->window_width / 2.f - len * scaling / 2.f, instance->window_height * 2.f / 3.f,
                std::string("Go to next map?"), Vector3(1.f), scaling);
        }
        else {
            len = 128.f;
            drawText(instance->window_width / 2.f - len * scaling / 2.f, instance->window_height * 2.f / 3.f,
                std::string("See tournament results?"), Vector3(1.f), scaling);
        }
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
        if (menu == MAP_SEL) instance->nextMap();
        break;
    case SDLK_LEFT:
        if (menu == MAP_SEL) instance->previousMap();
        break;
    case SDLK_RETURN:
        if (menu == MAIN) instance->goToStage(MAP_SEL_ST);
        else if (menu == MAP_SEL) instance->goToStage(PLAY_ST);
        else if (menu == DEATH) instance->goToStage(PLAY_ST);
        else if (menu == WIN) {
            uint8 tmp_map_counter = instance->currentMap;
            instance->nextMap();
            if (tmp_map_counter != instance->currentMap) {
                instance->goToStage(PLAY_ST);
            }
            else {
                instance->goToStage(MAP_SEL_ST); // TODO: CREDITS STAGE
            }
        }
        break;
    case SDLK_q:
        if (menu == MAIN) instance->must_exit = true;
        else if (menu == MAP_SEL) instance->goToStage(MAIN_MENU_ST);
        else if (menu == DEATH || menu == WIN) instance->goToStage(MAP_SEL_ST);
        break;
    }
}
