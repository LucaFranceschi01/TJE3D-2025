#include "entityMesh.h"

#include <algorithm>

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "framework/camera.h"
#include "graphics/texture.h"

#include "game/game.h"

constexpr auto CULLING_DIST = 400.f;

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
}

void EntityMesh::render(Camera* camera)
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
        // iterate through all instanced entities
        for (const Matrix44& model : models) {
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
        // ATTRIBUTES ARE PER INSTANCE, UNIFORMS ARE PER (mesh¿)
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
            for (auto& m : models) {
                mesh->renderBounding(m);
            }
        }
    }

    Entity::render(camera);
}

void EntityMesh::addInstance(const Matrix44& model)
{
    models.push_back(model);
}

void EntityMesh::addMeshLOD(Mesh* mesh, float distance)
{
    meshLods.push_back({mesh, distance});

    std::sort(meshLods.begin(), meshLods.end(),
        // Lambda function that compares two distances, largest distances come first
        [](const s_MeshLOD& a, const s_MeshLOD& b) {
            return a.distance > b.distance;
        });
}

Mesh* EntityMesh::getLOD(Camera* camera)
{
    // Get last frame LOD
    Mesh* current_lod = mesh;
    
    // Get distance from mesh to camera
    float distance = camera->eye.distance(getGlobalMatrix().getTranslation());

    // Adjust LOD depending on distance
    for (auto& lod : meshLods) {
        if (distance > lod.distance) {
            current_lod = lod.mesh;
            break;
        }
    }

    // Set current LOD
    mesh = current_lod;

    return current_lod;
}
