#include "menuStage.h"

#include "game/world.h"
#include "game/game.h"

#include "graphics/shader.h"

#include "framework/camera.h"
#include "framework/entities/entityUI.h"
#include "framework/input.h"

#include "framework/entities/particleEmitter.h"

#include <algorithm>

std::vector<std::string> button_descriptions = {
            "", "Move the player",
            "", "Move the player when split",
            "Split and join Don Bolon",
            "Change running direction",
            "Tipically means EXIT, BACK or PAUSE",
            "Resume playthrough",
            "Tipycally means GO or PLAY"
};

Vector2 btn_size(128.f, 64.f);
Vector2 btn_size_small(32.f, 32.f);
float offset = 10.f;


MenuStage::MenuStage(StageType menu)
{
    World* instance = World::getInstance();

    camera2D = new Camera();
    camera2D->view_matrix = Matrix44(); // Set View to identity, maybe not needed
    camera2D->setOrthographic(0.f, instance->window_width, instance->window_height, 0.f, -1.f, 1.f);

    stage_type = menu;
}

void MenuStage::init()
{
    World* instance = World::getInstance();
    float width = instance->window_width;
    float height = instance->window_height;

    UI_root = new Entity();
    background = new Entity();

    EntityUI* ui_elem;
    Material mat;
    Material mat_flat;
    mat_flat.color = { 0.f, 0.f, 0.f, 0.4 };
    mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

    // all menus have the same background
    ui_elem = new EntityUI(mat, Vector2(width / 2.f, height / 2.f),
        Vector2(width, height), "data/textures/ui/background.png");
    background->addChild(ui_elem);

    // all menus but the main menu have the background slightly greyed out
    if (stage_type != MAIN_MENU_ST) {
        ui_elem = new EntityUI(mat_flat, Vector2(width / 2.f, height / 2.f),
            Vector2(width, height));
        background->addChild(ui_elem);
    }

    // initialize UI elements and particle systems in each stage
    switch (stage_type)
    {
    case MAIN_MENU_ST:
    {   
        ui_elem = new EntityUI(mat, Vector2(width / 3.f, height * 5.f / 6.f),
            btn_size, "play", EntityUI::ENTER_INTRO);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, height * 5.f / 6.f),
            btn_size, "exit", EntityUI::EXIT);
        UI_root->addChild(ui_elem);

        break;
    }
    case MAP_SEL_ST:
    {   
        mapNames = { "M1_Tilted_Tenpins", "M2_Twisted_Lanes" , "M3_Tenpin_Crusher" };

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

        ui_elem = new EntityUI(mat, Vector2(width / 3.f, height * 5.f / 6.f),
            btn_size, "start", EntityUI::START_MAP);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, height * 5.f / 6.f),
            btn_size, "back", EntityUI::ENTER_INTRO);
        UI_root->addChild(ui_elem);

        break;
    }
    case DEATH_ST:
    {
        ui_elem = new EntityUI(mat, Vector2(width / 3.f, height * 5.f / 6.f),
            btn_size, "yes", EntityUI::START_MAP);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, height * 5.f / 6.f),
            btn_size, "no", EntityUI::ENTER_MAP_SELECTOR);
        UI_root->addChild(ui_elem);
        break;
    }
    case WIN_ST:
    {
        ui_elem = new EntityUI(mat, Vector2(width / 3.f, height * 5.f / 6.f),
            btn_size, "yes", EntityUI::NEXT_MAP);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, height * 5.f / 6.f),
            btn_size, "no", EntityUI::ENTER_MAP_SELECTOR);
        UI_root->addChild(ui_elem);
        
        confetti = new ParticleEmitter();
        confetti->setTexture("data/textures/particles/confetti.png");
        confetti->setTextureGridSize(5);
        confetti->setEmitRate(0.04f);
        confetti->setEmitVelocity(Vector3(0.f, 150.f, 0.f));
        confetti->setEmitPosition(Vector3(instance->window_width * 4.f / 7.f, 0.f, -0.5f));
        confetti->setMaxTimeAlive(8.f);
        confetti->setRandomFactor(instance->window_width);
        confetti->setSizesCurve({
            5.f,
            15.f
            });
        confetti->setColorsCurve({ Vector4(1.f, 1.f, 1.f, 1.f) });
        confetti->setAnimatedTexture(false);
        confetti->setEmissionEnabled(false);
        break;
    }
    case INTRO_ST:
    {
        ui_elem = new EntityUI(mat, Vector2(width / 3.f, 500.f),
            btn_size, "play", EntityUI::ENTER_MAP_SELECTOR);
        UI_root->addChild(ui_elem);

        ui_elem = new EntityUI(mat, Vector2(width * 2.f / 3.f, 500.f),
            btn_size, "exit", EntityUI::EXIT);
        UI_root->addChild(ui_elem);

        std::vector<std::vector<std::string>> button_names = {
            {"", "ub"}, {"lb", "db", "rb"},
            {"", "w"}, {"a", "s", "d"},
            {"e"}, {"z"}, {"q"}, {"r"}, {"enter"},
        };

        for (int i = 0; i < button_names.size(); i++) {
            for (int j = 0; j < button_names[i].size(); j++) {
                std::string& name = button_names[i][j];
                if (name == "") continue;
                ui_elem = new EntityUI(mat, Vector2(width / 7.f + j * (btn_size_small.x+offset), height / 6.f + i * (btn_size_small.y+offset)),
                    btn_size_small, name, EntityUI::TUTORIAL_BTN);
                UI_root->addChild(ui_elem);
            }
        }
        break;
    }
    default:
        break;
    }
}

void MenuStage::render()
{
    background->render(camera2D);

    if (confetti) confetti->render(camera2D);

    UI_root->render(camera2D);

    Game* instance = Game::instance;
    float scaling, len;

    switch (stage_type) {
        case MAP_SEL_ST:
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
        case DEATH_ST:
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
        case WIN_ST:
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
        case INTRO_ST:
        {
            float offset2;
            for (int i = 0; i < button_descriptions.size(); i++) {
                if (button_descriptions[i] == "") continue;

                if (i < 4) {
                    offset2 = btn_size_small.y;
                }
                else {
                    offset2 = offset / 2.f;
                }
                drawText(instance->window_width * 2.f / 7.f, instance->window_height / 6.f + i * (btn_size_small.y + offset) - offset2,
                    button_descriptions[i], Vector3(1.f), 2.f);
            }
            break;
        }
        default:
            break;
        }
}

void MenuStage::update(float dt)
{
    UI_root->update(dt);

    if (confetti) confetti->update(dt);
}

void MenuStage::onKeyDown(SDL_KeyboardEvent event)
{
    UI_root->onKeyDown(event);
    
    Game* instance = Game::instance;

    switch (event.keysym.sym) {
    case SDLK_RIGHT:
        if (stage_type == MAP_SEL_ST) instance->nextMap();
        break;
    case SDLK_LEFT:
        if (stage_type == MAP_SEL_ST) instance->previousMap();
        break;
    case SDLK_RETURN:
        if (stage_type == MAIN_MENU_ST) instance->goToStage(INTRO_ST);
        else if (stage_type == INTRO_ST) instance->goToStage(MAP_SEL_ST);
        else if (stage_type == MAP_SEL_ST) instance->goToStage(PLAY_ST);
        else if (stage_type == DEATH_ST) instance->goToStage(PLAY_ST);
        else if (stage_type == WIN_ST) {
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
        if (stage_type == MAIN_MENU_ST) instance->must_exit = true;
        else if (stage_type == INTRO_ST) instance->goToStage(MAIN_MENU_ST);
        else if (stage_type == MAP_SEL_ST) instance->goToStage(INTRO_ST);
        else if (stage_type == DEATH_ST || stage_type == WIN_ST) instance->goToStage(MAP_SEL_ST);
        break;
    }
}

void MenuStage::onEnter(Stage* prev_stage)
{
    if (confetti) confetti->setEmissionEnabled(true);
}

void MenuStage::onLeave(Stage* new_stage)
{
    if (confetti) {
        confetti->setEmissionEnabled(false);
        confetti->clearParticles();
    }
}
