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
    EntityUI* btn;
    Material mat;
    mat.color = { 0.f, 0.f, 0.f, 0.0f };

    for (int i = 0; i < World::getInstance()->live; i++) {
        btn = new EntityUI(mat, Vector2(width - 64.f * 3 + i * 64.f, 64.f),
            Vector2(64.f, 64.f), "data/textures/ui/LifeSprites.png", EntityUI::LIVES);
        UI_root->addChild(btn);
    }

    for (int i = 0; i < 10; i++) {
        btn = new EntityUI(mat, Vector2(32.f + i * 32.f, height - 32.f),
            Vector2(32.f, 64.f), "data/textures/ui/Pin.png", EntityUI::PIN_COUNTER);
        UI_root->addChild(btn);
    }

    btn = new EntityUI(mat, Vector2(width / 2.f, height / 2.f),
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

    btn = new EntityUI(mat, Vector2(width - 64.f * 3 + 2 * 64.f, 2 * 64.f),
        Vector2(64.f, 64.f), "data/textures/ui/frame.png", EntityUI::FRAME);
    btn->visible = true;
    UI_root->addChild(btn);

    btn = new EntityUI(mat, Vector2(width - 64.f * 3 + 2 * 64.f, 2 * 64.f - 2),
        Vector2(50.f, 50.f), "data/textures/ui/boosters.png", EntityUI::BOOSTER);
    btn->visible = true;
    UI_root->addChild(btn);

    current_booster = NONE_BOOSTER;
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

    // reset life bar 
    for (Entity* tmp : UI_root->children) {
        EntityUI* ui_element = static_cast<EntityUI*>(tmp);
        if (ui_element->buttonID == EntityUI::LIVES && !ui_element->visible) ui_element->visible = true;
    }

    pins_collected = 0;
}

void PlayStage::onLeave(Stage* prev_stage)
{
    // if game was paused unpause it (e.g. paused and exited a map, not resumed)
    if (Game::instance->paused) {
        switchPauseResume();
    }

    Game::instance->total_pins += pins_collected;
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
    World::getInstance()->onKeyDown(event);

    switch (event.keysym.sym) {
        case SDLK_TAB: // TODO: MOVE TO FUNCTION KEYS ALL THE DEBUG-RELATED FUNCTIONS
            Game::instance->setMouseLocked();
            break;
        case SDLK_RSHIFT:
            Game::instance->debug_view = !Game::instance->debug_view;
            break;
        case SDLK_q:
            if (!Game::instance->paused) switchPauseResume();
            else Game::instance->goToStage(MAP_SEL_ST);
            break;
        case SDLK_RETURN:
        case SDLK_r:
            if (Game::instance->paused) switchPauseResume();
            break;
        default:
            break;
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

        switch (ui_element->buttonID) {
            case EntityUI::PAUSE:
            case EntityUI::RESUME:
            case EntityUI::UNDEFINED:
            case EntityUI::ENTER_MAP_SELECTOR: {
                ui_element->visible = !ui_element->visible;
                break;
            }
            case EntityUI::FRAME:
            case EntityUI::BOOSTER: {
                ui_element->visible = !ui_element->visible;
                // maybe do more things
                break;
            }
            default:
                break;
        }

    }
}

void PlayStage::removeLifeUI()
{
    EntityUI* life = nullptr;
    for (Entity* tmp : UI_root->children) {
        EntityUI* ui_element = static_cast<EntityUI*>(tmp);
        if (ui_element->buttonID == EntityUI::LIVES && ui_element->visible) life = ui_element;
    }
    /*UI_root->removeChild(del);
    delete del;*/
    if (life) {
        life->visible = false;
    }
}

void PlayStage::collectPin()
{
    pins_collected++;
    Game::instance->map_pins[Game::instance->currentMap]++;

}
