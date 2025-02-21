#pragma once

#include <vector>
#include <string>

#include "entityMesh.h"

class Matrix44;
class Vector3;
class Material;
class EntityCollider;

struct sCollisionData
{
	Vector3 col_point;
	Vector3 col_normal;
	float distance = 3.4e+38F;
	bool collided = false;
	EntityCollider* collider = nullptr;
};

enum eColisionFilter : int
{
	ALL, OBSTACLE, GROUND, FLUID, NONE
};

class EntityCollider : public EntityMesh
{
private:
	void getCollisionsWithModel(const Matrix44& m, const Vector3& center, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& grounded_collision); // falten parametres

public:

	eColisionFilter layer = ALL;
	bool is_static = true; // hace falta?

	EntityCollider() = default;
	EntityCollider(Mesh* mesh, const Material& material, const std::string& name = "") : EntityMesh(mesh, material, name) {};

	void getCollisions(const Vector3& target_position, std::vector<sCollisionData>& colisions, std::vector<sCollisionData>& ground_colisions);
};