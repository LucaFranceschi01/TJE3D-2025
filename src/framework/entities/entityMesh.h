#pragma once

#include "entity.h"
#include "graphics/material.h"

class Mesh;

struct s_MeshLOD {
	Mesh* mesh;
	float distance;
};

class EntityMesh : public Entity {

public:

	EntityMesh();
	EntityMesh(Mesh* mesh, const Material& material);
	virtual ~EntityMesh() {};

	// Attributes of the derived class  
	Mesh* mesh;
	Material material;

	bool isInstanced;
	std::vector<Matrix44> models;
	std::vector<s_MeshLOD> lods;

	// Methods overwritten from base class
	void render(Camera* camera) override;
	void update(float elapsed_time) override;

	void addInstance(const Matrix44& model);
	/*
	TODO: EntityMesh::addMeshLOD(Mesh* mesh, float distance); // just pushes back lods with info
	also uses
	std::sort(mesh_lods.begin(), mesh_lods.end(), [](const meshLOD&)...
	*/
};
