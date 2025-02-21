//
// Created by Xavi Cañadas on 22/2/25.
//

#pragma once

#include "stage.h"


class MenuStage : public Stage
{
public:
    enum e_MenuID {
        MAIN, MAP_SEL, PAUSE, UNDEFINED
    };

    e_MenuID menu;

    Entity* UI_root = nullptr;
    Camera* camera2D = nullptr;

    // constructor
    MenuStage(e_MenuID menu);

    // methods
    void init() override;
    void render() override;
    void update(float dt) override;
    void onEnter(Stage* prev_stage) override;
    void onLeave(Stage* prev_stage) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onResize(int width, int height) override;
};