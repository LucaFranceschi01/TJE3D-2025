/*  by Xavier Canadas and Luca Franceschi 2025
Here we define the base Lore Stage class.
*/

#pragma once

#include "stage.h"
#include "framework/extra/bass.h"

class LoreStage : public Stage
{
public:
    Camera* camera2D = nullptr;
    std::vector<Entity*> images_lore;
    std::vector<std::string> text_lore;
    int index = 0;

    float text_timer = 0.0f;
    int current_char = 0;
    const float char_delay = 0.03f; 
    bool text_completed = false;

    HCHANNEL screen_sound;
    bool audio_stardet = false;

    // constructor
    LoreStage();
    ~LoreStage() = default;

    // methods
    void init() override;
    void render() override;
    void update(float dt) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onGamepadButtonDown(SDL_JoyButtonEvent event) override;
    void onEnter(Stage* prev_stage) override;
    void onLeave(Stage* new_stage) override;

    void next();
    void previous();
};




