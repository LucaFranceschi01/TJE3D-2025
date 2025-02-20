//
// Created by Xavi Cañadas on 3/2/25.
//
#include "stage.h"
#include "world.h"

#include "game/game.h"
#include "game/scene_parser.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

#include "framework/camera.h"
#include "framework/entities/entityMesh.h"
#include "framework/entities/entityCollider.h"
#include "framework/entities/entityUI.h"


PlayStage::PlayStage()
{
   /*
    entity_mesh = new EntityMesh(mesh, material);
    SceneParser parser;
    parser.parse("data/myscene.scene", root); // TODO: in blender do @player tag parser thing
    */
}

void PlayStage::init()
{
    World::getInstance()->init(World::TESTING);
}


void PlayStage::render()
{
    //root->render(camera);
    // Create model matrix for cube
    /*
    Matrix44 m;
    m.rotate(angle*DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));

    if (shader)
    {
        // Enable shader
        shader->enable();

        // Upload uniforms
        shader->setUniform("u_color", Vector4(1,1,1,1));
        shader->setUniform("u_viewprojection", camera->viewprojection_matrix );
        shader->setUniform("u_texture", texture, 0);
        shader->setUniform("u_model", m);
        shader->setUniform("u_time", time);

        // Do the draw call
        mesh->render( GL_TRIANGLES );

        // Disable shader
        shader->disable();
    }
    */

    World::getInstance()->render();
}

void PlayStage::update(float dt)
{
    World::getInstance()->update(dt);
}

void PlayStage::onEnter(Stage* prev_stage)
{
}

void PlayStage::onLeave(Stage* prev_stage)
{
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
    //World::getInstance()->player->onKeyDown(event);

    switch (event.keysym.sym) {
        case SDLK_TAB:
        {
            Game::instance->setMouseLocked();
            break;
        }
        case SDLK_RSHIFT:
        {
            Game::instance->debug_view = !Game::instance->debug_view;
            break;
        }
        case SDLK_SPACE:
        {
            // este codigo se tendra que borrar, es para probar la funcion de raycast de world
            Camera* camera = Camera::current;

            // get ray direction
            Vector3 ray_origin = camera->eye;
            Vector3 ray_direction = camera->getRayDirection(
                Input::mouse_position.x, Input::mouse_position.y,
                Game::instance->window_width, Game::instance->window_height
            );

            // test colision and get data
            sCollisionData data = World::getInstance()->raycast(ray_origin, ray_direction);

            // in case of colision, add el balon, to prove if works well
            if (data.collided) {
                Material player_material;
                player_material.diffuse = Texture::Get("data/meshes/faces.png");
                player_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

                EntityMesh* balon = new EntityMesh(Mesh::Get("data/player/Don_Bolon.obj"), player_material, "player");
                balon->model.setTranslation(data.col_point + Vector3(0.0f, 0.5f, 0.0f));
                World::getInstance()->addEntity(balon);
            }
        }
    }
}

void PlayStage::onResize(int width, int height)
{
    World* world = World::getInstance();
    world->camera->aspect = static_cast<float>(width) / static_cast<float>(height);
    //sColi
}

MenuStage::MenuStage(e_MenuID menu)
{
    World* instance = World::getInstance();

    camera2D = new Camera();
    camera2D->setOrthographic(0.f, instance->window_width, instance->window_height, 0.f, -1.f, 1.f);
    this->menu = menu;
}

void MenuStage::init()
{
    switch (menu)
    {
    case MenuStage::MAIN:
    {
        UI_root = new Entity();
        Material mat;

        mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
        EntityUI* btn = new EntityUI(Vector2(100.f, 100.f), Vector2(100.f, 100.f), EntityUI::e_UIButtonID::EXIT, mat, "exit");
        UI_root->addChild(btn);

        btn = new EntityUI(Vector2(300.f, 300.f), Vector2(100.f, 100.f), EntityUI::e_UIButtonID::START_MAP, mat, "play");
        UI_root->addChild(btn);
        break;
    }
    case MenuStage::MAP_SEL:
        break;
    case MenuStage::PAUSE:
        break;
    case MenuStage::UNDEFINED:
        break;
    default:
        break;
    }
    
    Stage::init();
}

void MenuStage::render()
{
    UI_root->render(camera2D);
    Stage::render();
}

void MenuStage::update(float dt)
{
    UI_root->update(dt);
    Stage::update(dt);
}

void MenuStage::onEnter(Stage* prev_stage)
{
    Stage::onEnter(prev_stage);
}

void MenuStage::onLeave(Stage* prev_stage)
{
}

void MenuStage::onKeyDown(SDL_KeyboardEvent event)
{
    Stage::onKeyDown(event);
}

void MenuStage::onResize(int width, int height)
{
    Stage::onResize(width, height);
}