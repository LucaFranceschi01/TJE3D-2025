//
// Created by Xavi Cañadas on 3/2/25.
//
#include "framework/input.h"

#include "game/game.h"
#include "game/scene_parser.h"

#include "stage.h"



PlayStage::PlayStage(Camera* camera) {
    this->camera = camera;
    // init root
    root = new Entity();
    //entity_mesh = new EntityMesh(mesh, material);
    SceneParser parser;
    parser.parse("data/myscene.scene", root); // TODO: in blender do @player tag parser thing
}

void PlayStage::render() {
    root->render(camera);
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

    // Draw the floor grid
    drawGrid();
}

void PlayStage::update(double dt) {
    float speed = dt * Game::instance->mouse_speed * 0.25; //the speed is defined by the seconds_elapsed so it goes constant

    // Example
    Game::instance->angle += static_cast<float>(dt) * 10.0f;

    // Mouse input to rotate the cam
    if (Input::isMousePressed(SDL_BUTTON_LEFT) || Game::instance->mouse_locked) //is left button pressed?
    {
        camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
        camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
    }

    // Async input to move the camera around
    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
    if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
    if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
    if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
    if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f,0.0f, 0.0f) * speed);
}

