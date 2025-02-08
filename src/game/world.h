//
// Created by Xavi Cañadas on 6/2/25.
//

#pragma once

#include <vector>

class Entity;
class EntityMesh;
class Player;
class Camera;

class World
{
private:
    static World* instance;
public:
    static World* getInstance();

    // attributes
    Entity* root = nullptr;
    EntityMesh* skybox = nullptr;
    Player* player = nullptr;
    Camera* camera = nullptr;

    float camera_yaw = 0.f;
    float camera_pitch = 0.f;
    float camera_speed = 2.0f;
    float mouse_speed = 0.25f;

    float window_height = 0.f;
    float window_width = 0.f;

    bool free_camera = true; // esto es para mover la cámara por el mapa (god mode)

    // constructor
    World();

    // methods
    void render();
    void update(float dt);

    // scene
    std::vector<Entity*> entities_to_destroy;

    void addEntity(Entity* entity);
    void destroyEntity(Entity* entity);
};
