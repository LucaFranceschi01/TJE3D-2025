#include "entityMesh.h"

#include <algorithm>

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "framework/camera.h"
#include "graphics/texture.h"

#include "game/game.h"

EntityMesh::EntityMesh() : Entity()
{
    mesh = nullptr;
    isInstanced = false;
}

EntityMesh::EntityMesh(Mesh* mesh, const Material& material, const std::string& name ) : EntityMesh()
{
    this->mesh = mesh;
    this->material = material;
    this->name = name;
    //this->materials = mesh->materials;
    //auto mat = mesh->materials.find("diffuse");
    //this->material.diffuse = mesh->materials.find("diffuse");
}

void EntityMesh::render(Camera* camera)
{
    if (!mesh) {
        assert(0);
        return;
    }

    // cullings
    bool must_render = true;
    std::vector<Matrix44> must_render_models;

    if (!isInstanced) {
        Matrix44 global_matrix = getGlobalMatrix(); // a nivel de mundo
        float distance = camera->eye.distance(global_matrix.getTranslation());

        must_render &= (distance < 200.0f);
        
        BoundingBox global_bb = transformBoundingBox(global_matrix, mesh->box);
        must_render &= (camera->testBoxInFrustum(global_bb.center, global_bb.halfsize) != CLIP_OUTSIDE);
    }
    else {
        float distance = 0.0f;
        Matrix44 global_matrix;
        Vector3 bb_center, bb_halfsize;
        for (const Matrix44& model : models) {
            global_matrix = model * parent->getGlobalMatrix();
            // same as getGlobalMatrix() but now "model" is not valid, we have to make this operation ourselves
            distance = camera->eye.distance(global_matrix.getTranslation());

            bb_center = global_matrix * mesh->box.center;
            bb_halfsize = mesh->box.halfsize;

            if (distance < 200.0f && (camera->testBoxInFrustum(bb_center, bb_halfsize) != CLIP_OUTSIDE)) {
                must_render_models.push_back(model);
            }
        }
    }

    if (!must_render) {
        return;
    }

    // end cullings

    if (!material.shader) {
        material.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs", "data/shaders/texture.fs");
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
        float distance = camera->eye.distance(getGlobalMatrix().getTranslation());

        Mesh* current_lod = mesh;

        for (auto& lod : meshLods) {
            if (distance > lod.distance) {
                current_lod = lod.mesh;
                break;
            }
        }

        //material.shader->setUniform("u_model", model);
        material.shader->setUniform("u_model", getGlobalMatrix());
        current_lod->render(GL_TRIANGLES);
    }
    else {
        // ATTRIBUTES ARE PER INSTANCE, UNIFORMS ARE PER mesh
        mesh->renderInstanced(GL_TRIANGLES, must_render_models.data(), static_cast<int>(must_render_models.size()));
    }

    //long now = SDL_GetTicks();
    //float time = float(now * 0.001);
    //material.shader->setUniform("u_time", time);

    // Disable shader after finishing rendering

    if (Game::instance->debug_view) {
        mesh->renderBounding(model);
    }

    material.shader->disable();

    // TODO: also do Entity::render()
    //Entity::render(camera);
}

void EntityMesh::update(float dt)
{
    Entity::update(dt);
}

void EntityMesh::addInstance(const Matrix44& model)
{
    models.push_back(model);
}

void EntityMesh::addMeshLOD(Mesh* mesh, float distance)
{
    meshLods.push_back({mesh, distance});

    std::sort(meshLods.begin(), meshLods.end(), [](const s_MeshLOD a, const s_MeshLOD& b) {
        return a.distance > b.distance;
    });
}

void EntityCollider::getCollisionsWithModel(const Matrix44& m, const Vector3& center,
    std::vector<sColisionData> colisions, std::vector<sColisionData> grounded_colision)
{
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
        colisions.push_back({colision_point, colision_normal, floor_sphere_center, true, nullptr});
        */

}

void EntityCollider::getCollisions(const Vector3& target_position, std::vector<sColisionData> colision_datas)
{
}
