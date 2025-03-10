#include "entityCollider.h"

#include <algorithm>
#include <valarray>

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "framework/camera.h"
#include "graphics/texture.h"

#include "game/game.h"
#include "game/world.h"
#include "game/player/player.h"

constexpr auto CULLING_DIST = 400.f;

void EntityCollider::render(Camera* camera)
{
    if (!mesh) {
        assert(0);
        return;
    }

    if (!is_visible) return;

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

    Shader* shader = material.shader;

    if (!shader) {
        shader = Shader::Get(
            isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs", "data/shaders/phong.fs");
    }

    // Enable shader and pass uniforms
    shader->enable();
    shader->setUniform("u_color", material.color);
    shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
    shader->setUniform("u_camera_position", camera->eye);
    shader->setUniform("u_background_color", Vector4(0.1f, 0.f, 0.f, 1.f));

    // Lighting
    shader->setUniform("u_Ka", Vector3(0.5f));
    shader->setUniform("u_Kd", Vector3(0.5f));
    shader->setUniform("u_Ks", Vector3(0.5f));
    shader->setUniform("u_light_color", Vector3(0.7f, 0.7f, 0.8f));
    shader->setUniform("u_light_position", World::getInstance()->player->getGlobalMatrix().getTranslation() + Vector3(0.f, 500.f, 200.f));
    shader->setUniform("u_fog_factor", 1.f);
    shader->setUniform("u_time", Game::instance->time);


    // Texture flags
    Vector2 maps = { 0.f, 0.f };

    if (material.diffuse) {
        maps.x = 1.0f;
        shader->setTexture("u_texture", material.diffuse, 0); // the slot is hardcoded
    }

    if (material.normal) { // we did not manage to make normals work...s
        maps.y = 1.0f;
        shader->setUniform("u_normal_texture", material.normal, 1);
    }

    shader->setUniform("u_maps", maps);

    if (!isInstanced) {
        // Send model matrix uniform and render current LOD mesh
        shader->setUniform("u_model", getGlobalMatrix());
        getLOD(camera)->render(GL_TRIANGLES);
    }
    else {
        // ATTRIBUTES ARE PER INSTANCE, UNIFORMS ARE PER (mesh)
        //shader->("a_color", material.color); // why cannot we do this ?
        mesh->renderInstanced(GL_TRIANGLES, must_render_models.data(), static_cast<int>(must_render_models.size()));
    }

    // Disable shader after finishing rendering
    shader->disable();

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
}

void EntityCollider::getCollisions(const Vector3& target_position,
    std::vector<sCollisionData>& colisions,
    std::vector<sCollisionData>& ground_colisions)
{
    if (!is_visible) return;

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

void EntityCollider::resetEntity()
{

    is_visible = true;

    if (!isInstanced || collision_type == PIN) return;

    for (auto& pair : models) {
        pair.second = true;
    }
}


