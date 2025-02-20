#include "entityUI.h"

#include <string>
#include <sstream>
#include <iostream>

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"
#include "graphics/texture.h"

EntityUI::EntityUI(Vector2 position, Vector2 size, e_UIButtonID buttonID, const Material& mat, const char* name)
{
	this->position = position;
	this->size = size;
	this->buttonID = buttonID;

	mesh = new Mesh();
	mesh->createQuad(position.x, position.y, size.x, size.y, true);

	this->material = mat;

	this->base = Texture::Get((std::string("data/textures/ui/") + std::string(name) + std::string("_base.png")).c_str()); // vaya basura
	this->pressed = Texture::Get((std::string("data/textures/ui/") + std::string(name) + std::string("_press.png")).c_str()); // vaya basura

	this->material.diffuse = this->base;

	if (!this->material.shader) {
		this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	}
}

void EntityUI::render(Camera* camera2D)
{
	if (!visible) return;

	// we do not have any 3d UI element, then we always
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// props to ChatGPT for these two lines of code
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	material.shader->enable();

	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_color", material.color); // TODO: CHECK IF SHADER USES U_COLOR
	material.shader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	material.shader->setUniform("u_camera_pos", camera2D->eye);

	if (material.diffuse) material.shader->setUniform("u_texture", material.diffuse, 0);

	mesh->render(GL_TRIANGLES);

	material.shader->disable();

	glEnable(GL_DEPTH_TEST);

	Entity::render(camera2D);
}

void EntityUI::update(float dt)
{
	Vector2 mouse_pos = Input::mouse_position;

	bool above_btn = (mouse_pos.x > position.x - size.x / 2.f) &&
		(mouse_pos.x < position.x + size.x / 2.f) &&
		(mouse_pos.y > position.y - size.y / 2.f) &&
		(mouse_pos.y < position.y + size.y / 2.f);

	if (above_btn && buttonID != e_UIButtonID::UNDEFINED) {

		material.color = Vector4(2.f);
		this->material.diffuse = this->pressed;

		if (Input::wasMousePressed(SDL_BUTTON_LEFT)) {
			Game* instance = Game::instance;


			switch (buttonID)
			{
			case EntityUI::ENTER_MAP_SELECTOR:
				instance->goToStage(MAP_SEL_ST);
				break;
			case EntityUI::EXIT:
				Game::instance->must_exit = true;
				break;
			case EntityUI::NEXT_MAP:
				// TODO: goToStage(...) or move the UI elements to the side
				break;
			case EntityUI::PREVIOUS_MAP:
				// TODO: goToStage(...) or move the UI elements to the side
				break;
			case EntityUI::START_MAP:
				instance->goToStage(PLAY_ST);
				break;
			case EntityUI::PAUSE:
				break;
			case EntityUI::RESUME:
				break;
			case EntityUI::UNDEFINED:
				break;
			default:
				break;
			}
		}
	}
	else {
		material.color = Vector4::WHITE;
		this->material.diffuse = this->base;
	}

	Entity::update(dt);
}
