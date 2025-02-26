#include "entityCollider.h"

#include "graphics/mesh.h"

void EntityCollider::getCollisionsWithModel(const Matrix44& m,
    const Vector3& center,
    std::vector<sCollisionData>& collisions,
    std::vector<sCollisionData>& grounded_collision)
{
    Vector3 colision_point;
    Vector3 colision_normal;

    float sphere_radius = 1.0f;

    // He hecho cambios en como se miran las colisiones. todas se ponen en el array de collision
    // y se diferencian con el collision filter
    if (mesh->testSphereCollision(m, center, sphere_radius, colision_point, colision_normal)) {
        collisions.push_back({colision_point, colision_normal.normalize(), -1, true, this});
    }

    // TODO: remove if not needed
    // de momento dejo este bloque de código que es el que habia antes
    /*
    // seguramente se tendran que canviar
    float sphere_radius = 1.2f;
    float sphere_ground_radius = 0.05f;
    float player_height = 0.3f;

    // check floor colisions
    Vector3 floor_sphere_center = center - Vector3(0.0f, 1, 0.0f);


    // hem de comprobar la colisio agafant el radi de la bola
    if (mesh->testSphereCollision(m, floor_sphere_center, sphere_ground_radius, colision_point, colision_normal)) {
        //float distance =
        grounded_colision.push_back({colision_point, colision_normal.normalize(), 1, true, this});
    }



        //se tiene que pasar el puntero del objeto actual como último atributo?

    // check wall colisions
    Vector3 character_center = center + Vector3(0.0f, player_height, 0.0f); // to not consider the ground colisions



    if (mesh->testSphereCollision(m, character_center, sphere_radius, colision_point, colision_normal))
        colisions.push_back({colision_point, colision_normal.normalize(), -1, true, this});


    */

    /*
    if (mesh->testRayCollision(m, center, Vector3(0, -1, 0), colision_point, colision_normal))
        colisions.push_back({colision_point, colision_normal.normalize(), -1, true, this});
        */
}

void EntityCollider::getCollisions(const Vector3& target_position,
    std::vector<sCollisionData>& colisions,
    std::vector<sCollisionData>& ground_colisions)
{
    Matrix44 global_matrix;

    if (!isInstanced) {
        global_matrix = getGlobalMatrix(); // a nivel de mundo
        getCollisionsWithModel(global_matrix, target_position, colisions, ground_colisions);
    }
    else {
        float distance = 0.0f;
        Vector3 bb_center, bb_halfsize;
        for (const Matrix44& model : models) {
            // same as getGlobalMatrix() but now "model" is not valid, we have to make this operation ourselves
            global_matrix = model * parent->getGlobalMatrix();
            getCollisionsWithModel(global_matrix, target_position, colisions, ground_colisions);
        }
    }
}
