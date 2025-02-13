#pragma once

#include "entity.h"
#include "graphics/material.h"

class Mesh;
//class Shader;

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
	virtual ~EntityMesh() {};

	// Attributes of the derived class  
	Mesh* mesh;
	Material material;
	//Shader* shader = nullptr;
	//std::map<std::string, sMaterialInfo> materials;

	bool isInstanced;
	std::vector<Matrix44> models;
	std::vector<s_MeshLOD> meshLods;

	// Methods overwritten from base class
	void render(Camera* camera) override;
	void update(float dt) override;

	// More methods
	void addInstance(const Matrix44& model);
	void addMeshLOD(Mesh* mesh, float distance);
};


class EntityCollider;

struct sColisionData
{
	Vector3 col_point;
	Vector3 col_normal;
	float distance = 3.4e+38F;
	bool collided = false;
	EntityCollider* collider = nullptr;
};

class EntityCollider : public EntityMesh
{
private:
	void getCollisionsWithModel(const Matrix44& m, const Vector3& center, std::vector<sColisionData> colisions, std::vector<sColisionData> grounded_colision); // falten parametres

public:
	EntityCollider() = default;
	EntityCollider(Mesh* mesh, const Material& material, const std::string& name = "") : EntityMesh(mesh, material, name) {};

	void getCollisions(const Vector3& target_position, std::vector<sColisionData> colision_datas);

};