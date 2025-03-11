/*  by Xavier Canadas and Luca Franceschi 2025
	Here we define the EntityUI derived class
*/

#pragma once

#include "entityMesh.h"
#include "game/player/player.h"

// Forward declarations
class Material;
class Texture;

class EntityUI : public EntityMesh
{
public:
	Vector2 position;
	Vector2 size;

	bool visible = true;
	bool is_pixel_art;

	enum e_UIButtonID {
		ENTER_MAP_SELECTOR,
		EXIT,
		NEXT_MAP,
		PREVIOUS_MAP,
		START_MAP,
		PAUSE,
		RESUME,
		MAP_THUMBNAIL,
		LIVES,
		PIN_COUNTER,
		TUTORIAL_BTN,
		ENTER_INTRO,
		ENTER_OUTRO,
		ENTER_MAIN_MENU,
		BOOSTER,
		FRAME,
		ENTER_LEADERBOARD,
		ENTER_LORE,
		LORE_NEXT,
		UNDEFINED
	};

	e_UIButtonID buttonID;
	uint8 mapID = NULL;
	uint8 pin_ID = NULL;
	eBooster booster_selected = NONE_BOOSTER; // I don't know if I'm going to use it

	Texture* base = nullptr;
	Texture* pressed = nullptr;

	bool is_button_pressed = false;

	// ctors
	EntityUI() = default;
	EntityUI(const Material& material, Vector2 position, Vector2 size,
		const std::string& name = "", e_UIButtonID buttonID = UNDEFINED);
	
	// dtor
	~EntityUI() = default;

	// Override virtual methods
	void render(Camera* camera2D) override;
	void update(float dt) override;

	void onKeyDown(SDL_KeyboardEvent event) override;
	void onKeyUp(SDL_KeyboardEvent event) override;
	void onGamepadButtonDown(SDL_JoyButtonEvent event) override;
	void onGamepadButtonUp(SDL_JoyButtonEvent event) override;
};
