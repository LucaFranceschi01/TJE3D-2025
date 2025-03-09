/*  by Xavier Canadas and Luca Franceschi 2025
	Here we define the EntityMesh class and the MeshLOD structure
*/

#pragma once

#include "entity.h"
#include "graphics/material.h"

// Forward declarations
class Mesh;

// Level Of Detail of mesh
struct s_MeshLOD
{
    Mesh* mesh;
    float distance;
};

class EntityMesh : public Entity
{
public:

	EntityMesh();
	EntityMesh(Mesh* mesh, const Material& material, const std::string& name = "");
	virtual ~EntityMesh() = default;

	// Attributes of the derived class  
	Mesh* mesh;
	Material material;

	// Attributes needed for instancing
	bool isInstanced;
	std::vector<std::pair<Matrix44,bool>> models;

	// Attributes needed for LOD
	std::vector<s_MeshLOD> meshLods;

	// Methods overwritten from base class
	void render(Camera* camera) override;

	// More methods
	void addInstance(const Matrix44& model);
	void addMeshLOD(Mesh* mesh, float distance);
	Mesh* getLOD(Camera* camera);

	virtual void onKeyDown(SDL_KeyboardEvent event) {};
};
