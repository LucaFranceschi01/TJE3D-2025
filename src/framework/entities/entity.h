/*  by Xavier Canadas and Luca Franceschi 2025
    Here we define the base Entity class
*/

#pragma once

#include "framework/framework.h" // includes string, vector, Matrix44
#include "framework/input.h"

// Forward declarations
class Camera;

class Entity
{
public:
    Entity() = default; // Constructor
    virtual ~Entity() = default; // Destructor

    std::string name;
    Matrix44 model;

    // Needed if we have nested entities
    Entity* parent = nullptr;
    std::vector<Entity*> children;

    // Methods of the base class
    virtual void addChild(Entity* child) final;
    virtual void removeChild(Entity* child) final;

    // Methods that should be overwritten by derived classes
    virtual void render(Camera* camera);
    virtual void update(float dt);

    // Some useful methods
    Matrix44 getGlobalMatrix() const;
    float distance(Entity* e) const;

    virtual void onKeyDown(SDL_KeyboardEvent event);
    virtual void onKeyUp(SDL_KeyboardEvent event);
    virtual void onGamepadButtonDown(SDL_JoyButtonEvent event);
    virtual void onGamepadButtonUp(SDL_JoyButtonEvent event);
};
