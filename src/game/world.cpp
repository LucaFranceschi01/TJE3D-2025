//
// Created by Xavi Cañadas on 6/2/25.
//

#include "world.h"
#include "game.h"
#include "scene_parser.h"
#include "player/player.h"

#include "framework/input.h"
#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityCollider.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "player/halfPlayer.h"

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

    // TODO: Init IN-GAME UI

    // Init player
    Material player_material;
    player_material.diffuse = Texture::Get("data/meshes/faces.png");
    player_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    player = new Player(Mesh::Get("data/player/Don_Bolon.obj"), player_material, "player");

    // Init half player
    half_player_left = new HalfPlayer(Mesh::Get("data/player/Don_Bolon.obj"), player_material, true, "player");
    half_player_right = new HalfPlayer(Mesh::Get("data/player/Don_Bolon.obj"), player_material, false, "player");


    for (int m = 0; m != UNDEFINED_MAP; m++) {
        e_MapID map = static_cast<e_MapID>(m);
        root[map] = new Entity();
    }

    {
        Texture* cube_texture = new Texture();
        /*
        cube_texture->loadCubemap("landscape", {
            "data/textures/cubemap/right.png",
            "data/textures/cubemap/left.png",
            "data/textures/cubemap/bottom.png",
            "data/textures/cubemap/top.png",
            "data/textures/cubemap/front.png",
            "data/textures/cubemap/back.png"
            });
        */

        cube_texture->loadCubemap("landscape", {
            "data/textures/cubemap/px.png",
            "data/textures/cubemap/nx.png",
            "data/textures/cubemap/ny.png",
            "data/textures/cubemap/py.png",
            "data/textures/cubemap/pz.png",
            "data/textures/cubemap/nz.png"
            });

        //Texture::Get("landscape"); una vez que se carga la texture, se puede acceder por todo el código con esto.

        // continuació

        Material cubemap_material;
        cubemap_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubemap.fs");
        cubemap_material.diffuse = cube_texture;

        skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.ASE"), cubemap_material);
    }
}

void World::init(e_MapID map)
{
    current_map = map;

    // Parse scene
    SceneParser parser;
    bool completed = parser.parse("data/myscene.scene", root[map]); // TODO: in blender do @player tag parser thing
    assert(completed);

    half_player = false;
}


void World::render()
{
    // set camera as default
    camera->enable();

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);


    glDisable(GL_DEPTH_TEST);
    skybox->render(camera);
    glEnable(GL_DEPTH_TEST);

    // draw the floor grid
    drawGrid();

    // render all scene tree
    root[current_map]->render(camera);

    // render entity player
    if (!half_player) {
        player->render(camera);
    } else {
        half_player_left->render(camera);
        half_player_right->render(camera);
    }


    // render life text. todo: es temporal
    std::string live_text = "lives: " + std::to_string(live) + "/3";
    drawText(700, 2, live_text, Vector3(1, 1, 1), 2);
}


void World::update(float dt)
{
    // update all elements in the scene
    root[current_map]->update(dt);

    // update the player
    if (!half_player) {
        player->update(dt);
    } else {
        half_player_left->update(dt);
        half_player_right->update(dt);
    }


    // update camera
    camera->update(dt);

    skybox->model.setTranslation(camera->eye);

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
    float up_factor = 3.5;

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
        Vector3 player_tr = Vector3(0.0f);
        if (!world->half_player) {
            player_tr = world->player->model.getTranslation();
            orbit_distance = 10;
            up_factor = 3.5;
        } else {
            player_tr = world->midPointHalfPlayers();
            orbit_distance = 20;
            up_factor = 10;
        }

        Vector3 eye = player_tr - World::front * orbit_distance + Vector3::UP * up_factor;



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
    // find to avoid inserting the same entity twice
    if (entities_to_destroy.find(entity) == entities_to_destroy.end()) {
        entities_to_destroy.insert(entity);
    }
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

/**
 * This method tries if a given position collides with any object in the world, in root.
 * For every entity calls getCollision.
 *
 * @param position position to look the collisions
 * @param colisions array of the collisions found in position.
 * @param ground_colisions colisions with the ground
 */
void World::test_scene_collisions(const Vector3& position, std::vector<sCollisionData>& colisions, std::vector<sCollisionData>& ground_colisions)
{
    for (auto entity : root[current_map]->children) {
        EntityCollider* entity_collider = dynamic_cast<EntityCollider*>(entity);

        if (entity_collider == nullptr)
            continue;

        entity_collider->getCollisions(position, colisions, ground_colisions);
    }
}

void World::onKeyDown(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym) {
        case SDLK_1:
        {
            game_mode = DEBUG;
            break;
        }
        case SDLK_2:
        {
            game_mode = RELEASE;
            break;
        }
        case SDLK_e:
        {
            if (!half_player) {
                half_player = true;
                half_player_left->init(player->model.getTranslation());
                half_player_right->init(player->model.getTranslation());
            } else {
                half_player = false;
                player->model.setTranslation(midPointHalfPlayers());
            }
            break;
        }
    }
}

Vector3 World::midPointHalfPlayers()
{
    Vector3 mid_point = Vector3(0.0f);

    // the two players will have the same x and y
    // the only thing that changes is the z, we decided to make the half point between the two.
    const Vector3 half_player_left_pos = half_player_left->model.getTranslation();
    const Vector3& half_player_right_pos = half_player_right->model.getTranslation();

    float half_point_x = (half_player_left_pos.x - half_player_right_pos.x) / 2;
    mid_point.x = half_point_x + half_player_right_pos.x;

    mid_point.y = half_player_left_pos.y;


    /*
    float half_point_z = (half_player_left_pos.z - half_player_right_pos.z) / 2;
    mid_point.z = half_point_z + half_player_right_pos.z;
    */
    mid_point.z = 0.0f;

    return mid_point;
}


// TODO: este codigo estaba en el constructor

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