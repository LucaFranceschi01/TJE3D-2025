#include "entityMesh.h"

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "framework/camera.h"

EntityMesh::EntityMesh() : Entity()
{
	//material = ...
	//shader = material.shader;
}

EntityMesh::EntityMesh(Mesh* mesh, const Material& material)
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
		material.shader = Shader::Get("", "");
	}
	
	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	material.shader->enable();
	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_viewproj", camera->viewprojection_matrix);
	material.shader->setTexture("u_texture", material.diffuse, 0);
	
	// material.shader->setTexture("u_texture", material.normals, 1);

	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	material.shader->disable();

	// TODO: also do Entity::render()
	Entity::render(camera);
}

void EntityMesh::update(float elapsed_time)
{
	Entity::update(elapsed_time);
}
