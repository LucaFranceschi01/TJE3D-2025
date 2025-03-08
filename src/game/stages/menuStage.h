/*  by Xavier Cañadas and Luca Franceschi 2025
    Here we define the PlayStage class which is a menu with some UI
*/

#pragma once

#include "stage.h"

class MenuStage : public Stage
{
public:
    enum e_MenuID {
        MAIN, MAP_SEL, PAUSE, DEATH, WIN, UNDEFINED
    };

    e_MenuID menu;

    Entity* UI_root = nullptr;
    Camera* camera2D = nullptr;

    std::vector<std::string> mapNames;

    // constructor
    MenuStage(e_MenuID menu);
    ~MenuStage() = default;

    // methods
    void init() override;
    void render() override;
    void update(float dt) override;

    void onKeyDown(SDL_KeyboardEvent event) override;
};
