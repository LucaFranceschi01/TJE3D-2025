#include "entityUI.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"


EntityUI::EntityUI(Vector2 position, Vector2 size, e_UIButtonID buttonID, const Material& mat)
{
	this->position = position;
	this->size = size;
	this->buttonID = buttonID;

	mesh = new Mesh();
	mesh->createQuad(position.x, position.y, size.x, size.y, false);

	this->material = mat;

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

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	}

	Entity::update(dt);
}
