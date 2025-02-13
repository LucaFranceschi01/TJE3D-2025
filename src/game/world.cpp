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
#include "framework/entities/entityMesh.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

World* World::instance = nullptr;

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

    root = new Entity();

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

    {
        Texture* cube_texture = new Texture();
        cube_texture->loadCubemap("landscape", {
            "data/meshes/cubemap/right.png",
            "data/meshes/cubemap/left.png",
            "data/meshes/cubemap/bottom.png",
            "data/meshes/cubemap/top.png",
            "data/meshes/cubemap/front.png",
            "data/meshes/cubemap/back.png"
        });

        //Texture::Get("landscape"); una vez que se carga la texture, se puede acceder por todo el código con esto.

        // continuació

        Material cubemap_material;
        cubemap_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubemap.fs"); // falla la textura. Se tiene que pedir el cubemap.fs por si es diferente
        cubemap_material.diffuse = cube_texture;

        skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.ASE"), cubemap_material);
    }



    // render player
    Material player_material;
    player_material.diffuse = Texture::Get("data/meshes/faces.png");
    player_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

    player = new Player(Mesh::Get("data/player/Don_Bolon.obj"), player_material, "player");

    SceneParser parser;
    bool completed = parser.parse("data/myscene.scene", root); // TODO: in blender do @player tag parser thing
    assert(completed);
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
    //drawGrid();

    // render entity player
    player->render(camera);

    // render all scene tree
    //root->render(camera);
}


void World::update(float dt)
{
    // update all elements
    root->update(dt);

    // update the player
    player->update(dt);

    float speed = dt * Game::instance->mouse_speed * 1; //the speed is defined by the seconds_elapsed so it goes constant

    if (free_camera) {
        // camara libre
        // Example
        Game::instance->angle += static_cast<float>(dt) * 10.0f;

        // Mouse input to rotate the cam
        if (Input::isMousePressed(SDL_BUTTON_LEFT) || Game::instance->mouse_locked) {
            //is left button pressed?
            camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
            camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
        }

        // Async input to move the camera around
        if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
        if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f,0.0f, 0.0f) * speed);
    } else {
        // camara en 3a persona
        // get camera delta
        camera_yaw -= Input::mouse_delta.x * dt * mouse_speed;
        camera_pitch -= Input::mouse_delta.y * dt * mouse_speed;

        // restrict pitch angle
        camera_pitch = clamp(camera_pitch, -M_PI * 0.4f, M_PI * 0.4f);

        Matrix44 mYaw;
        mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));

        Matrix44 mPitch;
        mPitch.setRotation(camera_pitch, Vector3(-1, 0, 0));

        Vector3 front = (mPitch * mYaw).frontVector().normalize();
        Vector3 eye;
        Vector3 center;

        // camera 3rd person
        float orbit_distance = 10.5f;
        eye = player->model.getTranslation() - front * orbit_distance;
        center = player->model.getTranslation() + Vector3(0.f, 0.5f, 0.0f);

        camera->lookAt(eye, center, Vector3::UP);
    }
    // hay cosas ...
    skybox->model.setTranslation(camera->eye);

    // delete pending entities
    for (auto entity : entities_to_destroy) {
        root->removeChild(entity);
        delete entity;
    }
    entities_to_destroy.clear();
}


void World::addEntity(Entity* entity)
{
    root->addChild(entity);
}


void World::destroyEntity(Entity* entity)
{
    entities_to_destroy.push_back(entity);
}

