//
// Created by Xavi Cañadas on 6/2/25.
//

#include "world.h"
#include "game.h"
#include "scene_parser.h"
#include "player.h"

#include "framework/input.h"
#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityCollider.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

World* World::instance = nullptr;

const Vector3 World::front(1.f, 0.f, 0.f);
const Vector3 World::right(0.f, 0.f, 1.f);
const Vector3 World::up(0.f, 1.f, 0.f);

World* World::getInstance()
{
    if (instance == nullptr) {
        instance = new World();
    }
    return instance;
}

World::World()
{
    window_height = static_cast<float>(Game::instance->window_height);
    window_width = static_cast<float>(Game::instance->window_width);

    camera = new Camera();
    camera->lookAt(Vector3(0.f, 10.f, 10.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f));
    //position the camera and point to 0,0,0
    camera->setPerspective(72.f, window_width / window_height, 0.1f, 10000.f);

    /*root = new Entity();*/

    // create height map
    // esto lo pongo pq lo ha puesto en clase, ns si lo usaremos
    /*
    {
        float size = 100.0f;
        Mesh* heightmap_mesh = new Mesh();
        heightmap_mesh->createSubdividedPlane(size);

        Material heightmap_material = {
            Shader::Get("data/shaders/height_map.vs", "data/shaders/height_map.fs"),
            Vector4(1.0f, 1.0f, 1.0f, 1.0f),
            Texture::Get("data/textures/heightmap.png")
        };

        EntityMesh* heightmap = new EntityMesh(heightmap_mesh ,heightmap_material);
        heightmap->model.translate(-size * 0.5f, 0.0f, -size * 0.5f);
        root->addChild(heightmap);
    }
    */

    // esto inicializa el sky, lo comento de momento

    /*
    {
        Texture* cube_texture = new Texture();
        cube_texture->loadCubemap("landscape", {
            "data/textures/cubemap/right.png",
            "data/textures/cubemap/left.png",
            "data/textures/cubemap/bottom.png",
            "data/textures/cubemap/top.png",
            "data/textures/cubemap/front.png",
            "data/textures/cubemap/back.png"
        });

        //Texture::Get("landscape"); una vez que se carga la texture, se puede acceder por todo el código con esto.

        // continuació

        Material cubemap_material;
        cubemap_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubemap.fs");
        cubemap_material.diffuse = cube_texture;

        skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.ASE"), cubemap_material);
    }
    */

    // TODO: Init IN-GAME UI

    // Init player
    Material player_material;
    player_material.diffuse = Texture::Get("data/meshes/faces.png");
    player_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    player = new Player(Mesh::Get("data/player/Don_Bolon.obj"), player_material, "player");

    for (int m = 0; m != UNDEFINED_MAP; m++) {
        e_MapID map = static_cast<e_MapID>(m);
        root[map] = new Entity();
    }
}

void World::init(e_MapID map)
{
    current_map = map;

    // Parse scene
    SceneParser parser;
    bool completed = parser.parse("data/myscene.scene", root[map]); // TODO: in blender do @player tag parser thing
    assert(completed);
}


void World::render()
{
    // set camera as default
    camera->enable();

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);


    /*
    glDisable(GL_DEPTH_TEST);
    skybox->render(camera);
    glEnable(GL_DEPTH_TEST);
    */


    // draw the floor grid
    drawGrid();

    // render entity player
    player->render(camera);

    // render all scene tree
    root[current_map]->render(camera);
}


void World::update(float dt)
{
    // update all elements in the scene
    root[current_map]->update(dt);

    // update the player
    player->update(dt);

    // update camera
    camera->update(dt);


    //skybox->model.setTranslation(camera->eye);

    // delete pending entities
    for (auto entity : entities_to_destroy) {
        root[current_map]->removeChild(entity);
        delete entity;
    }
    entities_to_destroy.clear();
}

void Camera::update(float dt)
{
    Camera * camera = Camera::current;
    World* world = World::getInstance();
    
    float speed = dt * Game::instance->mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant
    float orbit_distance = 8.0f;

    switch (world->camera_mode)
    {
    case World::FREE:
    {
        // camara libre
        // Example
        Game::instance->angle += dt * 10.0f;

        // Mouse input to rotate the cam
        if (Input::isMousePressed(SDL_BUTTON_LEFT) || Game::instance->mouse_locked) {
            //is left button pressed?
            camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
            camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
        }

        // Async input to move the camera around
        if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
        if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
        break;
    }
    case World::FIXED:
    {
        // TODO
        break;
    }
    case World::THIRD_PERSON:
    {
        Vector3 player_tr = world->player->model.getTranslation();

        Vector3 eye = player_tr - World::front * orbit_distance + Vector3::UP * 3.5f;

        camera->lookAt(eye, player_tr, Vector3::UP);

        break;
    }
    default:
    {
        break;
    }
    }
}

void World::addEntity(Entity* entity)
{
    root[current_map]->addChild(entity);
}


void World::destroyEntity(Entity* entity)
{
    entities_to_destroy.push_back(entity);
}

sCollisionData World::raycast(const Vector3& origin, const Vector3& direction, int layer, bool closest, float max_ray)
{
    sCollisionData colision_data = {};
    // ns si hay algo encima del for

    for (auto entity : root[current_map]->children) {

        EntityCollider* entity_collider = dynamic_cast<EntityCollider*>(entity);

        if (entity_collider == nullptr || !(entity_collider->layer & layer))
            continue;

        Vector3 colision_point, colision_normal;

        if (!entity_collider->isInstanced) {

            if (!entity_collider->mesh->testRayCollision(entity_collider->model, origin, direction, colision_point, colision_normal))
                continue;

            // there was a colision
            float new_distance = static_cast<float>((colision_point - origin).length());

            if (new_distance < colision_data.distance)
                colision_data = {colision_point, colision_normal, new_distance, true, entity_collider};

            if (!closest)
                return colision_data;

        } else {
            for (const Matrix44& model : entity_collider->models) {

                if (!entity_collider->mesh->testRayCollision(model, origin, direction, colision_point, colision_normal))
                    continue;

                float new_distance = static_cast<float>((colision_point - origin).length());

                if (new_distance < colision_data.distance)
                    colision_data = {colision_point, colision_normal, new_distance, true, entity_collider};

                if (!closest)
                    return colision_data;
            }
        }
    }
    return colision_data;
}

void World::test_scene_collisions(const Vector3& position, std::vector<sCollisionData>& colisions, std::vector<sCollisionData>& ground_colisions)
{
    for (auto entity : root[current_map]->children) {
        EntityCollider* entity_collider = dynamic_cast<EntityCollider*>(entity);

        if (entity_collider == nullptr)
            continue;

        entity_collider->getCollisions(position, colisions, ground_colisions);
    }
}
