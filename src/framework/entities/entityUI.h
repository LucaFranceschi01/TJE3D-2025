#pragma once

#include <string>
#include "entityMesh.h"
#include "framework/framework.h"

class Material;
class Texture;

class EntityUI : public EntityMesh
{
public:
	Vector2 position;
	Vector2 size;
	//bool is3D = false;
	bool visible = true;
	//Vector3 pos3d;

	enum e_UIButtonID {
		ENTER_MAP_SELECTOR,
		EXIT,
		NEXT_MAP,
		PREVIOUS_MAP,
		START_MAP,
		PAUSE,
		RESUME,
		UNDEFINED
	};

	e_UIButtonID buttonID = UNDEFINED;

	Texture* base = nullptr;
	Texture* pressed = nullptr;

	// update3d not needed

	EntityUI(Vector2 position, Vector2 size, e_UIButtonID buttonID, const Material& mat, const char* name);
	~EntityUI() {};

	void render(Camera* camera2D) override;
	void update(float dt) override;

	EntityUI() = default;
};

//update3d
//visible true
//
//project position3d 
//
//if in the frustum render
//
//dentro del menustage tiene background y 