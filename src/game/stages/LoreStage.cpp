//
// Created by Xavi Cañadas on 11/3/25.
//

#include "LoreStage.h"

#include "framework/audio.h"
#include "framework/camera.h"
#include "framework/entities/entityUI.h"
#include "game/game.h"
#include "game/world.h"

LoreStage::LoreStage()
{
    Game* instance = Game::instance;
    int width = instance->window_width;
    int height = instance->window_height;

    camera2D = new Camera();
    camera2D->view_matrix = Matrix44(); // Set View to identity, maybe not needed
    camera2D->setOrthographic(0.f, width, height, 0.f, -1.f, 1.f);


    stage_type = LORE_ST;

    Material mat;

    for (int i = 0; i < 3; ++i) {
        EntityUI* ui_elem = new EntityUI(mat, Vector2(width / 2.f, height / 2.f),
                                         Vector2(height, height),
                                         "data/textures/lore_" + std::to_string(i) + ".png");
        images_lore.push_back(ui_elem);
    }

    text_lore.emplace_back(
            "Pepe Strikazos es el mejor jugador de la IBF, y tiene un oscuro\n"
            "secreto... realmente no sabe jugar a bolos. La verdadera razon\n"
            "de su exito es Don Bolon, su especial bola de bolos que por\n"
            "pura casualidad se encontro de pequeno. Realmente es la 'bola'\n"
            "poliforme parlanchina la que juega por el."
            );

    text_lore.emplace_back(
            "Don Bolon es capaz de moverse a su antojo sin que nadie\n"
            "se de cuenta, logrando asi que Pepe Strikazos haya ascendido\n"
            "a lo mas alto de los torneos de bolos, siendo una leyenda\n"
            "en el mundillo."
            );
    text_lore.emplace_back(
            "En esta edicion de la World Bowling Championship, Don Bolon\n"
            "tendra que lograr las mejores puntuaciones posibles para que\n"
            "Pepe Strikazos no pierda su titulo."
            );
}

void LoreStage::init()
{
    index = 0;
    Stage::init();
}

void LoreStage::render()
{
    Game* instance = Game::instance;

    const float window_height = static_cast<float>(instance->window_height);
    images_lore[index]->render(camera2D);

    float offset = 100.f;
    float scaling = 2.f;
    drawText(60, window_height - 150,
                 text_lore[index], Vector3(1.f), scaling);

    Stage::render();
}

void LoreStage::update(float dt)
{
    Stage::update(dt);
}

void LoreStage::onKeyDown(SDL_KeyboardEvent event)
{
    Game* instance = Game::instance;

    // TODO: put mouse control. I don't remeber how to do it.
    switch (event.keysym.sym) {
        case SDLK_RIGHT:
        case SDLK_RETURN:
        case SDLK_d: {
            index++;

            if (index > 2) {
                instance->goToStage(INTRO_ST);
            }
            break;
        }
        case SDLK_LEFT:
        case SDLK_a:
        case SDLK_q: {
            index--;

            if (index < 0) {
                instance->goToStage(MAIN_MENU_ST);
            }
            break;
        }
        default: {
            break;
        }
    }
    Stage::onKeyDown(event);
}

void LoreStage::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
    Stage::onGamepadButtonDown(event);
}

void LoreStage::onEnter(Stage* prev_stage)
{
    Stage::onEnter(prev_stage);
}

void LoreStage::onLeave(Stage* new_stage)
{
    index = 0;

    Stage::onLeave(new_stage);

}
