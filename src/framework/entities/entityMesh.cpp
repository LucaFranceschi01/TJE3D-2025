#include "entityMesh.h"

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "framework/camera.h"
#include "graphics/texture.h"

EntityMesh::EntityMesh() : Entity()
{
	mesh = nullptr;
	isInstanced = false;
}

EntityMesh::EntityMesh(Mesh* mesh, const Material& material) : EntityMesh()
{
	this->mesh = mesh;
	this->material = material;
}

void EntityMesh::render(Camera* camera) {

	if ( !mesh ) {
		assert(0, "No mesh was loaded");
		return;
	}

	if ( !material.shader ) {
		material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	}
	
	// Get the last camera that was activated 
	//Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	material.shader->enable();
	
	material.shader->setUniform("u_color", material.color);

	material.shader->setUniform("u_viewproj", camera->viewprojection_matrix);

	if (!material.diffuse) {
		material.diffuse = Texture::Get("data/textures/missing.tga");
	}

	material.shader->setTexture("u_texture", material.diffuse, 0); // the slot is hardcoded
	// material.shader->setTexture("u_texture", material.normals, 1);

	if (!isInstanced) {
		material.shader->setUniform("u_model", model);
		mesh->render(GL_TRIANGLES);
	}
	else {
		mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	}
	
	//long now = SDL_GetTicks();
	//float time = float(now * 0.001);
	//material.shader->setUniform("u_time", time);

	// Disable shader after finishing rendering
	material.shader->disable();

	// TODO: also do Entity::render()
	Entity::render(camera);
}

void EntityMesh::update(float elapsed_time)
{
	Entity::update(elapsed_time);
}
