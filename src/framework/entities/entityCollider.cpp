#include "entityCollider.h"

void EntityCollider::getCollisionsWithModel(const Matrix44& m, const Vector3& center,
    std::vector<sCollisionData> colisions, std::vector<sCollisionData> grounded_colision)
{
    // TODO: se tiene que preguntar esta funcion porque ns que poner.
    Vector3 colision_point;
    Vector3 colision_normal;

    // seguramente se tendran que canviar
    float sphere_radius = 0.15f;
    float sphere_ground_radius = 0.05f;
    float player_height = 0.2f;

    // check floor colisions
    Vector3 floor_sphere_center = center + Vector3(0.0f, sphere_ground_radius, 0.0f);

    /*
    if (mesh->testSphereCollision(m, floor_sphere_center, sphere_radius, colision_point, colision_normal))
        colisions.push_back({colision_point, colision_normal.normalize(), floor_sphere_center, true, ????});
    */

    // check wall colisions
    Vector3 character_center = center + Vector3(0.0f, player_height, 0.0f); // aqui seria solo center por ser la bola?

    /*
    if (mesh->testSphereCollision(m, character_center, sphere_radius, colision_point, colision_normal))
        colisions.push_back({colision_point, colision_normal.normalize(), character_center, true, ????});

    if (mesh->testRayCollision(m, character_center, Vector3(0, -1, 0), colision_point, colision_normal))
        colisions.push_back({colision_point, colision_normal.normalize(), character_center, true, ????});
    */



}

// TODO: se tiene que preguntar
void EntityCollider::getCollisions(const Vector3& target_position, std::vector<sCollisionData>& colisions, std::vector<sCollisionData>& ground_colisions)
{
    //if (!layer & filter)
}
