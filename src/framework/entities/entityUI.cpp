#include "entityUI.h"

#include "framework/audio.h"
#include "framework/camera.h"
#include "framework/input.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

#include "game/game.h"
#include "game/stages/playStage.h"
#include "game/stages/LoreStage.h"

#include "game/world.h"

unsigned int pin_counter = 0;

/**
* If the buttonID is undefined (no interaction) the string name is taken literally.
* If it is not, it will be used to load _base and _press textures.
*/
EntityUI::EntityUI(const Material& material, Vector2 position, Vector2 size,
	const std::string& name, e_UIButtonID buttonID)
{
	this->material = material;
	this->position = position;
	this->size = size;
	this->buttonID = buttonID;
	this->name = name;
	
	mesh = new Mesh();
	mesh->createQuad(position.x, position.y, size.x, size.y, true); // uvs are flipped in the horizontal axis

	if (name != "") {

		switch (buttonID) {
			case UNDEFINED:
			case LORE_NEXT:
			case MAP_THUMBNAIL: {
				this->is_pixel_art = false;
				this->base = Texture::Get(name.c_str());
				break;
			}
			case LIVES: {
				this->is_pixel_art = false;
				this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/lives.fs");
				this->base = Texture::Get(name.c_str());
				break;
			}
			case PIN_COUNTER: {
				this->is_pixel_art = false;
				this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pins.fs");
				this->base = Texture::Get(name.c_str());
				this->pin_ID = pin_counter++;
				break;
			}
			case BOOSTER: {
				this->is_pixel_art = false;
				this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/boosters.fs");
				this->base = Texture::Get(name.c_str());
				this->booster_selected = NONE_BOOSTER;
				break;
			}
			case FRAME: {
				this->is_pixel_art = false;
				this->base = Texture::Get(name.c_str());
				this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/frame.fs");
				break;
			}
			default: {
				// frame here
				this->is_pixel_art = true;
				this->base = Texture::Get((std::string("data/textures/ui/") + name + std::string("_base.png")).c_str());
				this->pressed = Texture::Get((std::string("data/textures/ui/") + name + std::string("_press.png")).c_str());
			}
		}

		/*
		if ((buttonID == UNDEFINED || buttonID == MAP_THUMBNAIL)) {
			this->is_pixel_art = false;
			this->base = Texture::Get(name.c_str());
		}
		else if (buttonID == LIVES) {
			this->is_pixel_art = false;
			this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/lives.fs");
			this->base = Texture::Get(name.c_str());
		}
		else if (buttonID == PIN_COUNTER) {
			this->is_pixel_art = false;
			this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pins.fs");
			this->base = Texture::Get(name.c_str());
			this->pin_ID = pin_counter++;
		}
		else {
			this->is_pixel_art = true;
			this->base = Texture::Get((std::string("data/textures/ui/") + name + std::string("_base.png")).c_str());
			this->pressed = Texture::Get((std::string("data/textures/ui/") + name + std::string("_press.png")).c_str());
		}
		*/
	}

	this->material.diffuse = this->base;



	if (!this->material.shader) {
		this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	}
}

void EntityUI::render(Camera* camera2D)
{
	if (!visible) return;

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	material.shader->enable();

	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	material.shader->setUniform("u_camera_pos", camera2D->eye);
	material.shader->setUniform("u_lives", World::getInstance()->live);
	material.shader->setUniform("u_pins_collected", Game::instance->map_pins[Game::instance->currentMap]);
	material.shader->setUniform("u_pin_ID", pin_ID);
	material.shader->setUniform("u_booster_selected", static_cast<float>(World::getInstance()->player->booster));
	material.shader->setUniform("u_choosing_booster", World::getInstance()->player->choosing_booster);
	material.shader->setUniform("u_time_choosing_booster", World::getInstance()->player->time_choosing_booster);
	material.shader->setUniform("u_time_booster", World::getInstance()->player->time_booster);
	material.shader->setUniform("u_time", Game::instance->time); // I don't know if I'm going to use it

	if (material.diffuse) material.shader->setUniform("u_texture", material.diffuse, 0);

	if (is_pixel_art) {
		// props to ChatGPT for these two lines of code
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	mesh->render(GL_TRIANGLES);

	material.shader->disable();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	Entity::render(camera2D);
}

void EntityUI::update(float dt)
{
	Vector2 mouse_pos = Input::mouse_position;

	bool above_btn = (mouse_pos.x > position.x - size.x / 2.f) &&
		(mouse_pos.x < position.x + size.x / 2.f) &&
		(mouse_pos.y > position.y - size.y / 2.f) &&
		(mouse_pos.y < position.y + size.y / 2.f);

	Game* instance = Game::instance;

	if (above_btn
		&& buttonID != EntityUI::UNDEFINED
		&& buttonID != EntityUI::MAP_THUMBNAIL
		&& buttonID != EntityUI::LIVES
		&& buttonID != EntityUI::PIN_COUNTER) {

		if (buttonID != EntityUI::LORE_NEXT) this->material.diffuse = this->pressed;

		if (Input::wasMousePressed(SDL_BUTTON_LEFT)) {
			//Audio::Play("data/sounds/click.mp3");
			switch (buttonID)
			{
			case EntityUI::ENTER_MAP_SELECTOR:
				instance->goToStage(MAP_SEL_ST);
				break;
			case EntityUI::ENTER_INTRO:
				instance->goToStage(INTRO_ST);
				break;
			case EntityUI::ENTER_LORE:
				instance->goToStage(LORE_ST);
				break;
			case EntityUI::EXIT:
				if (instance->currentStage->stage_type == MAIN_MENU_ST) {
					instance->must_exit = true;
				}
				else if (instance->currentStage->stage_type == INTRO_ST) {
					instance->goToStage(MAIN_MENU_ST);
				}
				else if (instance->currentStage->stage_type == MAP_SEL_ST) {
					instance->goToStage(INTRO_ST);
				}
				else if (instance->currentStage->stage_type == OUTRO_ST) {
					instance->goToStage(MAIN_MENU_ST);
				}
				break;
			case EntityUI::NEXT_MAP:
			{
				uint8 tmp_map_counter = instance->currentMap;
				instance->nextMap();
				if (instance->currentStage->stage_type == StageType::WIN_ST) {
					if (tmp_map_counter != instance->currentMap) {
						instance->goToStage(PLAY_ST);
					}
					else {
						instance->goToStage(OUTRO_ST);
					}
				}
				break;
			}
			case EntityUI::PREVIOUS_MAP:
				instance->previousMap();
				break;
			case EntityUI::START_MAP:
				instance->goToStage(PLAY_ST);
				break;
			case EntityUI::PAUSE:
				instance->currentStage->switchPauseResume();
				break;
			case EntityUI::RESUME:
				instance->currentStage->switchPauseResume();
				break;
			case EntityUI::ENTER_LEADERBOARD:
				instance->goToStage(OUTRO_ST);
				break;
			case EntityUI::LORE_NEXT:
			{
				LoreStage* lore = dynamic_cast<LoreStage*>(instance->currentStage);
				if (!lore) break;

				lore->next();
				break;
			}
			default:
				break;
			}
		}
	}
	else {
		this->material.diffuse = this->base;
	}

	if (this->is_button_pressed) {
		this->material.diffuse = this->pressed;
	}

	if (buttonID == MAP_THUMBNAIL) {
		uint8 current_map = instance->currentMap;

		//position.x = instance->window_width / 2.f + (mapID - current_map) * 512.f;
		model.setTranslation((mapID - current_map) * 400 - current_map, model.getTranslation().y, 0.f);
	}

	Entity::update(dt);
}

std::map<int, std::string> event_mapping_btn{
	{SDLK_q, "q"},
	{SDLK_w, "w"},
	{SDLK_e, "e"},
	{SDLK_r, "r"},
	{SDLK_a, "a"},
	{SDLK_s, "s"},
	{SDLK_d, "d"},
	{SDLK_z, "z"},
	{SDLK_RETURN, "enter"},
	{SDLK_UP, "ub"},
	{SDLK_LEFT, "lb"},
	{SDLK_DOWN, "db"},
	{SDLK_RIGHT, "rb"},
};

std::map<int, std::string> event_mapping_gp{
	{0, "gp_a"},
	{1, "gp_b"},
	{2, "gp_x"},
	{3, "gp_y"},
	{4, "gp_lb"},
	{5, "gp_rb"},
	{6, "gp_view"},
	{7, "gp_menu"},
};

void EntityUI::onKeyDown(SDL_KeyboardEvent event)
{
	//event_mapping[event.keysym.sym]
	auto result = event_mapping_btn.find(event.keysym.sym);

	if (result == event_mapping_btn.end()) return;

	this->is_button_pressed = false;
	if (name == result->second) {
		this->is_button_pressed = true;
	}

	Entity::onKeyDown(event);
}

void EntityUI::onKeyUp(SDL_KeyboardEvent event)
{
	this->is_button_pressed = false;
}

void EntityUI::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
	auto result = event_mapping_gp.find(event.button);

	if (result == event_mapping_gp.end()) return;

	this->is_button_pressed = false;
	if (name == result->second) {
		this->is_button_pressed = true;
	}
}

void EntityUI::onGamepadButtonUp(SDL_JoyButtonEvent event)
{
	this->is_button_pressed = false;
}
