/*  by Xavier Cañadas and Luca Franceschi 2025
    Here we define the PlayStage class which is an in-game stage
*/

#pragma once

#include "stage.h"
#include "framework/extra/bass.h"

class RenderToTexture;

enum eBooster : int;

class PlayStage : public Stage
{
public:
    Entity* UI_root = nullptr;
    Camera* camera2D = nullptr;
    eBooster current_booster;



    RenderToTexture* postprocessing = nullptr;

    // ctor and dtor
    PlayStage();
    ~PlayStage() = default;

    // methods
    void init() override;
    void render() override;
    void update(float dt) override;
    void onEnter(Stage* prev_stage) override;
    void onLeave(Stage* prev_stage) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onGamepadButtonDown(SDL_JoyButtonEvent event) override;
    void onResize(int width, int height) override;

    void switchPauseResume() override;
    void removeLifeUI() override;
    void addLiveUI() override;
    void collectPin() override;
};
