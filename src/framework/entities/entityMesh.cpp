#include "entityMesh.h"

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "framework/camera.h"

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

void EntityMesh::render() {

	if ( !mesh ) {
		assert(0);
		return;
	}

	if ( !material.shader ) {
		material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	}
	
	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	material.shader->enable();
	
	if (!isInstanced) {
		material.shader->setUniform("u_model", model);
	}
	
	if (material.diffuse) {
		// TODO: set texture
		material.shader->setTexture("u_texture", material.diffuse, 0); // the slot is hardcoded
	} // TODO: ELSE poner con missing ???

	material.shader->setUniform("u_viewproj", camera->viewprojection_matrix);
	// material.shader->setTexture("u_texture", material.normals, 1);

	if (isInstanced) {
		mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	}
	else {
		mesh->render(GL_TRIANGLES);
	}

	// Disable shader after finishing rendering
	material.shader->disable();

	// TODO: also do Entity::render()
	Entity::render(camera);
}

void EntityMesh::update(float elapsed_time)
{
	Entity::update(elapsed_time);
}
