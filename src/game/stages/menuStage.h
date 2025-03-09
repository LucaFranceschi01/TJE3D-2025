/*  by Xavier Cañadas and Luca Franceschi 2025
    Here we define the PlayStage class which is a menu with some UI
*/

#pragma once

#include "stage.h"

class ParticleEmitter;

class MenuStage : public Stage
{
public:
    Entity* UI_root = nullptr;
    Entity* background = nullptr;
    Camera* camera2D = nullptr;

    ParticleEmitter* confetti = nullptr;

    std::vector<std::string> mapNames;

    // constructor
    MenuStage(StageType menu);
    ~MenuStage() = default;

    // methods
    void init() override;
    void render() override;
    void update(float dt) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onEnter(Stage* prev_stage) override;
    void onLeave(Stage* new_stage) override;
};
