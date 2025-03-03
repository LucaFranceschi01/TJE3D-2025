/*  by Xavier Cañadas and Luca Franceschi 2025
    Here we define the World class which is a singleton with all the information related to a 3d scene
    including player, maps, camera, etc.
*/

#pragma once

#include <vector>
#include <map>
#include <set>

#include "framework/framework.h"
#include "framework/entities/entityCollider.h"

class Entity;
class EntityMesh;
class Player;
class HalfPlayer;
class Camera;

class World
{
private:
    static World* instance;
public:
    static World* getInstance();

    enum e_camera_mode {
        FREE,
        FIXED,
        THIRD_PERSON,
        NR_CAMERA_MODES
    };

    // move the player free with awsd or default only right to left
    enum e_game_mode {
        DEBUG, RELEASE
    };

    // attributes
    //Entity* root = nullptr;
    EntityMesh* skybox = nullptr;

    Camera* camera = nullptr;

    // players
    Player* player = nullptr;
    HalfPlayer* half_player_left = nullptr;
    HalfPlayer* half_player_right = nullptr;
    bool half_player = false;
    int live = 3; // esto lo he puesto a aqui para no tenerlo duplicado en el half_player

    uint8 current_map;
    std::map<uint8, Entity*> root;

    float camera_yaw = 0.f;
    float camera_pitch = 0.f;
    float camera_speed = 2.0f;
    float mouse_speed = 0.25f;

    float window_height = 0.f;
    float window_width = 0.f;

    e_camera_mode camera_mode = THIRD_PERSON;
    e_game_mode game_mode = DEBUG;

    static Vector3 front;
    static Vector3 right;
    static const Vector3 up;

    // constructor
    World();

    // methods
    void render();
    void update(float dt);
    void init();

    // scene
    std::set<Entity*> entities_to_destroy;

    void addEntity(Entity* entity);
    void destroyEntity(Entity* entity);

    // colisions
    sCollisionData raycast(const Vector3& origin, const Vector3& direction, int layer = eColisionFilter::ALL, bool closest = true, float max_ray = 1000); // ns q es max_ray
    void test_scene_collisions(const Vector3& position, std::vector<sCollisionData>& colisions, std::vector<sCollisionData>& ground_colisions);

    void onKeyDown(SDL_KeyboardEvent event);

    Vector3 midPointHalfPlayers();
};
