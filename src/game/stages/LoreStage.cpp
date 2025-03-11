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
            Vector2(height, height), "data/textures/lore/lore_" + std::to_string(i) + ".png", EntityUI::LORE_NEXT);
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
    text_timer = 0.0f;
    current_char = 0;
    text_completed = false;
    audio_stardet = false;
    Stage::init();
}

void LoreStage::render()
{
    Game* instance = Game::instance;

    const float window_height = static_cast<float>(instance->window_height);
    images_lore[index]->render(camera2D);

    float offset = 100.f;
    float scaling = 2.f;

    std::string partial_text;
    if (current_char > 0) {
        partial_text = text_lore[index].substr(0, current_char);
    }
    
    drawText(60, window_height - 150, partial_text, Vector3(1.f), scaling);

    Stage::render();
}

void LoreStage::update(float dt)
{
    // Prompt claude sonnet: only show one char at time
    if (!text_completed) {

        if (!audio_stardet) {
            screen_sound = Audio::Play("data/sounds/r2d2.mp3", 0.05, BASS_SAMPLE_LOOP);
            audio_stardet = true;
        }

        text_timer += dt;
        
        int total_chars = static_cast<int>(text_lore[index].length());
        int chars_to_show = static_cast<int>(text_timer / char_delay);
        
        if (chars_to_show > current_char) {
            current_char = chars_to_show;
            
            if (current_char >= total_chars) {
                text_completed = true;
                current_char = total_chars;
                audio_stardet = false;
                Audio::Stop(screen_sound);
            }
        }
    }

    images_lore[index]->update(dt);

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
            next();
            break;
        }
        case SDLK_LEFT:
        case SDLK_a:
        case SDLK_q: {
            previous();
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
    index = 0;
    text_timer = 0.0f;
    current_char = 0;
    text_completed = false;
    audio_stardet = false;
    Stage::onEnter(prev_stage);
}

void LoreStage::onLeave(Stage* new_stage)
{
    index = 0;
    Audio::Stop(screen_sound);
    Stage::onLeave(new_stage);

}

void LoreStage::next()
{
    if (!text_completed) {
        current_char = text_lore[index].length();
        text_completed = true;
        audio_stardet = false;
        Audio::Stop(screen_sound);
    }
    else {
        index++;
        if (index > 2) {
            Game::instance->goToStage(INTRO_ST);
        }
        else {
            text_timer = 0.0f;
            current_char = 0;
            text_completed = false;

        }
    }
}

void LoreStage::previous()
{
    if (!text_completed) {
        current_char = text_lore[index].length();
        text_completed = true;
        audio_stardet = false;
        Audio::Stop(screen_sound);
    }
    else {
        index--;
        if (index < 0) {
            Game::instance->goToStage(MAIN_MENU_ST);
        }
        else {
            // Reinicia las variables para el nuevo texto
            text_timer = 0.0f;
            current_char = 0;
            text_completed = false;
        }
    }
}
