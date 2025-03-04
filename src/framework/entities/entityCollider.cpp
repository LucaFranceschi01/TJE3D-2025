#include "entityCollider.h"

#include <algorithm>
#include <valarray>

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "framework/camera.h"
#include "graphics/texture.h"

#include "game/game.h"

constexpr auto CULLING_DIST = 400.f;

void EntityCollider::render(Camera* camera)
{
    if (!mesh) {
        assert(0);
        return;
    }

    // =================== START CULLINGS ===================

    bool must_render = true;
    std::vector<Matrix44> must_render_models;
    Matrix44 global_matrix;
    float distance;
    BoundingBox global_bb;

    if (!isInstanced) {
        global_matrix = getGlobalMatrix(); // a nivel de mundo
        distance = camera->eye.distance(global_matrix.getTranslation());

        must_render &= (distance < CULLING_DIST);
        
        global_bb = transformBoundingBox(global_matrix, mesh->box); // adjusts bounding box
        must_render &= (camera->testBoxInFrustum(global_bb.center, global_bb.halfsize) != CLIP_OUTSIDE);
    }
    else {
        if (layer == PIN) {
            int a = 0;
        }



        // iterate through all instanced entities
        for (const auto& pair : models) {
            if (!pair.second) continue; // jump over the models that are not active
            const Matrix44& model = pair.first;

            // same as getGlobalMatrix() but now "model" is not valid, we have to make this operation ourselves
            global_matrix = model * parent->getGlobalMatrix();
            distance = camera->eye.distance(global_matrix.getTranslation());

            global_bb = transformBoundingBox(global_matrix, mesh->box);
            
            if (distance < CULLING_DIST &&
                (camera->testBoxInFrustum(global_bb.center, global_bb.halfsize) != CLIP_OUTSIDE))
            {
                must_render_models.push_back(model);
            }
        }
    }

    if (!must_render) {
        return;
    }

    // =================== END CULLINGS ===================

    if (!material.shader) {
        material.shader = Shader::Get(
            isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs", "data/shaders/texture.fs");
    }

    // Enable shader and pass uniforms
    material.shader->enable();

    material.shader->setUniform("u_color", material.color);
    material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
    material.shader->setUniform("u_camera_position", camera->eye);

    if (material.diffuse) {
        material.shader->setTexture("u_texture", material.diffuse, 0); // the slot is hardcoded
    }

    if (!isInstanced) {
        // Send model matrix uniform and render current LOD mesh
        material.shader->setUniform("u_model", getGlobalMatrix());
        getLOD(camera)->render(GL_TRIANGLES);
    }
    else {
        // ATTRIBUTES ARE PER INSTANCE, UNIFORMS ARE PER (mesh)
        //material.shader->("a_color", material.color); // why cannot we do this ?
        mesh->renderInstanced(GL_TRIANGLES, must_render_models.data(), static_cast<int>(must_render_models.size()));
    }

    // Disable shader after finishing rendering
    material.shader->disable();

    // Render bounding boxes if debugging mode is set
    if (Game::instance->debug_view) {
        if (!isInstanced) {
            mesh->renderBounding(model);
        } else {
            for (auto& pair : models) {
                Matrix44& m = pair.first;
                mesh->renderBounding(m);
            }
        }
    }

    Entity::render(camera);
}



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
        for (const auto& pair : models) {
            if (!pair.second) continue;
            const Matrix44& model = pair.first;
            // same as getGlobalMatrix() but now "model" is not valid, we have to make this operation ourselves
            global_matrix = model * parent->getGlobalMatrix();
            getCollisionsWithModel(global_matrix, target_position, colisions, ground_colisions);
        }
    }
}

int EntityCollider::smallestDistance(Vector3 point)
{

    if (!isInstanced) return -1;

    float distance = 99999999.f;
    int j = 0;

    for (int i = 0; i < models.size(); i++) {
        float current_distance = std::abs(models[i].first.getTranslation().distance(point));
        if (distance > current_distance) {
            distance = current_distance;
            j = i;
        }
    }
    return j;
}

void EntityCollider::resetInstanced()
{
    if (!isInstanced) return;

    for (auto& pair : models) {
        pair.second = true;
    }
}


