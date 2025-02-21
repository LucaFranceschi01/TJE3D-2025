//
// Created by Xavi Cañadas on 22/2/25.
//

#pragma once

#include "stage.h"


class PlayStage : public Stage
{
public:
    // constructor
    PlayStage();

    // methods
    void init() override;
    void render() override;
    void update(float dt) override;
    void onEnter(Stage* prev_stage) override;
    void onLeave(Stage* prev_stage) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
    void onResize(int width, int height) override;
};



