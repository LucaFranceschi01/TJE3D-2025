/*  by Xavier Cañadas and Luca Franceschi 2025
    Here we define the Player class and some helper functions for the movement
*/

#pragma once

#include "framework/entities/entityMesh.h"

class EntityUI;

enum eBooster: int
{
    INMORTAL = 0, SPIKES = 1, EXTRA_LIVE = 2, NONE_BOOSTER = 3
};


class Player : public EntityMesh
{
protected:
    float yaw = 0.f;
    float pitch = 0.f;
    float walk_speed = 2.f;
    float rotational_speed = 30.f;
    Vector3 velocity = Vector3(0.f);

    float jump_time = 0.0f;

    Vector3 front = Vector3(0.f);
    Vector3 right = Vector3(0.0f);
    Vector3 normal_orig;
    Vector3 move_dir;

public:
    bool collision = false;
    bool collision_fluid = false;
    float time_colision_fluid = 0;
    float time_fluid_i = 0;
    int fluid_factor = 0;

    eBooster booster = NONE_BOOSTER;
    float time_booster = 0;
    EntityUI* booster_ui = nullptr;
    EntityUI* frame_booster_ui = nullptr;
    bool choosing_booster = false;
    float time_choosing_booster = 0;

    Player() = default;
    Player(Mesh* mesh, const Material& material, const std::string& name);

    ~Player() = default;

    virtual void init(const Vector3& pos);

    void render(Camera* camera) override;
    void update(float dt) override;
    virtual void moveControl(Vector3& move_dir, float dt);

    bool testCollisions(const Vector3& position, float dt);
    void renderDebug(Camera* camera);
};

// Helper functions
static void dampen(float* deg);
inline static float positive_modulo(float i, float n);
