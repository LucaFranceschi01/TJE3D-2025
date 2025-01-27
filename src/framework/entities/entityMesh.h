#pragma once

#include "entity.h"
#include "graphics/material.h"

class Mesh;

class EntityMesh : public Entity {

public:

	EntityMesh();
	EntityMesh(Mesh* mesh, const Material& material);
	virtual ~EntityMesh() {};

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Material material; // TODO: implement this material

	bool isInstanced = false;
	std::vector<Matrix44> models;

	// Methods overwritten from base class
	void render();
	void update(float elapsed_time);
};
