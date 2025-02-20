//
// Created by Xavi Cañadas on 6/2/25.
//

#pragma once

#include <vector>
#include <map>
#include "framework/framework.h"
#include "framework/entities/entityCollider.h"

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

    enum e_MapID {
        TESTING, MAP1, UNDEFINED_MAP
    };

    enum e_camera_mode {
        FREE,
        FIXED,
        THIRD_PERSON,
        NR_CAMERA_MODES
    };

    // attributes
    //Entity* root = nullptr;
    EntityMesh* skybox = nullptr;
    Player* player = nullptr;
    Camera* camera = nullptr;
    Camera* camera2D = nullptr;

    e_MapID current_map;
    std::map<e_MapID, Entity*> root;

    float camera_yaw = 0.f;
    float camera_pitch = 0.f;
    float camera_speed = 2.0f;
    float mouse_speed = 0.25f;

    float window_height = 0.f;
    float window_width = 0.f;

    e_camera_mode camera_mode = THIRD_PERSON;

    static const Vector3 front;
    static const Vector3 right;
    static const Vector3 up;

    // constructor
    World();

    // methods
    void render();
    void update(float dt);
    void init(e_MapID map);

    // scene
    std::vector<Entity*> entities_to_destroy;

    void addEntity(Entity* entity);
    void destroyEntity(Entity* entity);

    // colisions
    sCollisionData raycast(const Vector3& origin, const Vector3& direction, int layer = sColisionFilter::ALL, bool closest = true, float max_ray = 1000); // ns q es max_ray
    void test_scene_collisions(const Vector3& position, std::vector<sCollisionData>& colisions, std::vector<sCollisionData>& ground_colisions);

};
