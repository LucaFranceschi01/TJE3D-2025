#include "particleEmitter.h"

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"

#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/input.h"

#include <cassert>
#include <algorithm>

ParticleEmitter::ParticleEmitter()
{
	particles.resize(max_particles);
}

void ParticleEmitter::setTexture(const char* filename)
{
	texture = Texture::Get(filename);
}

void ParticleEmitter::render(Camera* camera)
{
	assert(texture && "No texture in emitter!");

	// Mesh to pack all particle vertices
	Mesh quad;

	if (sort_distance)
	{
		std::sort(particles.begin(), particles.end(), [&](const sParticle& a, const sParticle& b) {
			return a.position.distance(camera->eye) > b.position.distance(camera->eye);
			});
	}

	// {0, 0}	, {0.5, 0}
	// {0, 0.5}	, {0.5, 0.5}
	std::vector<Vector2> uv_offsets;
	/*for (int j = 1; j <= texture_grid_size; ++j)
		for (int i = 0; i < texture_grid_size; ++i)
			uv_offsets.push_back(Vector2(i / (float)texture_grid_size, 1.f - j / (float)texture_grid_size));*/
	for (int i = 0; i < texture_grid_size; i++) {
		uv_offsets.push_back(Vector2(i / static_cast<float>(texture_grid_size), 0.f));
	}

	for (sParticle& p : particles)
	{
		if (!p.active)
			continue;

		Vector3 pos = p.position;

		float nt = clamp(p.ttl / max_ttl, 0.f, 1.f);

		Vector4 color = sample<Vector4>(nt, colors.size(), &colors[0]);
		float size = sample<float>(nt, sizes.size(), &sizes[0]);

		Vector3 right = camera->getLocalVector(Vector3(1, 0, 0));
		Vector3 top = camera->getLocalVector(Vector3::UP);

		//int grid_frames = (texture_grid_size * texture_grid_size) - 1;
		int grid_frames = texture_grid_size;
		int frame_index = animated_texture ? nt * grid_frames : p.id % grid_frames;
		float d_uvs = 1.f / texture_grid_size;
		Vector2 uv_offset = uv_offsets[frame_index];

		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top - right) * size);
		quad.uvs.push_back(Vector2(0, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top + right) * size);
		quad.uvs.push_back(Vector2(1, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);

		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top + right) * size);
		quad.uvs.push_back(Vector2(1, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (top + right) * size);
		quad.uvs.push_back(Vector2(1, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
	}

	if (!quad.vertices.size()) {
		return;
	}

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	if (additive_blending) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glDepthMask(false);

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	// Enable shader
	shader->enable();

	// Upload uniforms
	shader->setUniform("u_color", Vector4::WHITE);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_background_color", Vector4(0.1, 0.1, 0.1, 1.f));
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_tiling", 1.f);
	shader->setUniform("u_model", model);
	if (texture) shader->setUniform("u_texture", texture, 0);
	
	if (is_pixel_art) {
		// props to ChatGPT for these two lines of code
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}


	quad.render(GL_TRIANGLES);

	shader->disable();

	glDepthMask(true);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ParticleEmitter::update(float delta_time)
{
	emit_timer += delta_time;
	if (emission_enabled && emit_timer > emit_rate && active_particles < max_particles)
	{
		emit();
	}

	for (sParticle& p : particles)
	{
		if (!p.active)
			continue;

		Vector3 random_velocity = Vector3(random(random_factor, -random_factor * 0.48), 0.f, 0.f); // IS THIS RANDOM SLIGHTLY BIASED ???
		p.position += (p.velocity + random_velocity) * delta_time;
		//p.position.z = -0.5f;
		p.ttl += delta_time;

		// Kill particle if run out of time
		if (p.ttl > max_ttl) {
			p.active = false;
			active_particles--;
		}
	}
}

void ParticleEmitter::clearParticles()
{
	particles.clear();
	particles.resize(max_particles);
	emit_timer = 0.0f;
	active_particles = 0;
}

void ParticleEmitter::emit()
{
	emit_timer = 0.f;

	// Search slot and activate particle
	for (sParticle& p : particles)
	{
		if (p.active)
			continue;

		p.id = last_id++;
		p.ttl = 0.f;
		p.active = true;
		Vector3 random_pos = Vector3(random(random_factor, -random_factor * 0.48f), 0.f, 0.f);
		p.position = emit_position + random_pos;
		p.velocity = emit_velocity;
		active_particles++;
		break;
	}
}
