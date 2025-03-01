/*  by Xavier Cañadas and Luca Franceschi 2025
	Here we define the EntityUI derived class
*/

#pragma once

#include "entityMesh.h"

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
		UNDEFINED
	};

	e_UIButtonID buttonID;
	uint8 mapID = NULL;

	Texture* base = nullptr;
	Texture* pressed = nullptr;

	// ctors
	EntityUI() = default;
	EntityUI(const Material& material, Vector2 position, Vector2 size,
		const std::string& name = "", e_UIButtonID buttonID = UNDEFINED);
	
	// dtor
	~EntityUI() = default;

	// Override virtual methods
	void render(Camera* camera2D) override;
	void update(float dt) override;
};
