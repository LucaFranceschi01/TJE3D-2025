//
// Created by Xavi Cañadas on 3/2/25.
//

#pragma once

#include "framework/input.h"

class EntityUI;
class Entity;
class Camera;

typedef enum {
    MAIN_MENU_ST, MAP_SEL_ST, PLAY_ST, DEATH_ST, WIN_ST, UNDEFINED_ST
} StageType;

class Stage
{
public:
    virtual ~Stage() = default; // TODO: does this really matter since virtual ???

    // methods
    virtual void init() {};
    virtual void render() {};
    virtual void update(float dt) {};

    // events
    virtual void onKeyDown(SDL_KeyboardEvent event) {};
    virtual void onKeyUp(SDL_KeyboardEvent event) {};
    virtual void onMouseButtonDown(SDL_MouseButtonEvent event) {};
    virtual void onMouseButtonUp(SDL_MouseButtonEvent event) {};
    virtual void onMouseWheel(SDL_MouseWheelEvent event) {};
    virtual void onGamepadButtonDown(SDL_JoyButtonEvent event) {};
    virtual void onGamepadButtonUp(SDL_JoyButtonEvent event) {};
    virtual void onResize(int width, int height) {};
    virtual void onEnter(Stage* prev_stage) {};
    virtual void onLeave(Stage* new_stage) {};
};



/*

Tendriamos que hacer el loop del juego, que cuando empiece la partida, la bola este al principio del mapa
y que solo avance para adelante, yo quitaria las funciones de aumentar y disminuir la velocidad, o ns si quieres
que pueda parar lo dejamos.


Para esta primera entrega podriamos hacer un mapa sin nada estetico, q solo sea bloques o colores basicos para
implementar los obstaculos y todas las cosas. Dedicar lo menos posible al mapa en render ahora para poder implementarlo.

Despues, cuando funcione todas las cosas, hacer el mapa final y dedicando tiempo.



 */



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
