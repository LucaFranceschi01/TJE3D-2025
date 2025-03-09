/*  by Xavier Canadas and Luca Franceschi 2025
	Here we define the EntityCollider derived class
*/

#pragma once

#include "entityMesh.h"

// Forward declarations
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
	ALL, OBSTACLE, GROUND, FLUID, PIN, BOOSTER, FINISHLINE, NONE
};

class EntityCollider : public EntityMesh
{
private:
	void getCollisionsWithModel(const Matrix44& m, const Vector3& center, std::vector<sCollisionData>& collisions,
		std::vector<sCollisionData>& grounded_collision); // TODO: falten parametres

public:

	eColisionFilter layer = ALL;
	bool is_static = true; // TODO: hace falta?
	bool collision = false;
	bool is_visible = true;

	EntityCollider() = default;
	EntityCollider(Mesh* mesh, const Material& material, const std::string& name = "") : EntityMesh(mesh, material, name) {};
	
	~EntityCollider() = default;

	void render(Camera* camera) override;

	void getCollisions(const Vector3& target_position,
		std::vector<sCollisionData>& colisions,
		std::vector<sCollisionData>& ground_colisions);

	int smallestDistance(Vector3 point);
	void resetEntity();
};
