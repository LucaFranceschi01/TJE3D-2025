#include "entityCollider.h"

#include "graphics/mesh.h"


void EntityCollider::getCollisionsWithModel(const Matrix44& m, const Vector3& center,
                                            std::vector<sCollisionData>& colisions, std::vector<sCollisionData>& grounded_colision)
{
    // TODO: se tiene que preguntar esta funcion porque ns que poner.
    Vector3 colision_point;
    Vector3 colision_normal;

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


    /*
    if (mesh->testRayCollision(m, center, Vector3(0, -1, 0), colision_point, colision_normal))
        colisions.push_back({colision_point, colision_normal.normalize(), -1, true, this});
        */



}

// TODO: se tiene que preguntar

// aqui s'ha de diferenciar entre instanced i no instanced
void EntityCollider::getCollisions(const Vector3& target_position, std::vector<sCollisionData>& colisions, std::vector<sCollisionData>& ground_colisions)
{
    //if (!layer & filter)

    // aqui supongo que se llama a la funcion de arriba.


    // esta funcion sirve para ver si la target position colisiona con este objeto?

    // los dos arrays de colisions lo unico que se hace aqui es que si colisiona el objeto actual con la target colision,
    // se añade este objeto al array. Asi cuando se llame desde world esta funcion por todos los objetos de root,
    // el array se llenara.


    if (!isInstanced) {
        Matrix44 global_matrix = getGlobalMatrix(); // a nivel de mundo
        getCollisionsWithModel(global_matrix, target_position, colisions, ground_colisions);
    }
    else {
        float distance = 0.0f;
        Vector3 bb_center, bb_halfsize;
        for (const Matrix44& model : models) {
            Matrix44 global_matrix = model * parent->getGlobalMatrix();
            // same as getGlobalMatrix() but now "model" is not valid, we have to make this operation ourselves
            getCollisionsWithModel(global_matrix, target_position, colisions, ground_colisions);
        }
    }

}

