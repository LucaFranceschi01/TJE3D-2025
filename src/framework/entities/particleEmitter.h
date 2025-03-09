/*  Class provided by https://github.com/jxarco
*/

#pragma once

#include "entity.h"

#include "framework/includes.h"
#include "framework/framework.h"

class Texture;
class Camera;

struct sParticle {
	int id = -1;
	Vector3 position;
	Vector3 velocity;
	float ttl = 0.0f;
	float rotation = 0.0f;
	bool active = false;
};

class ParticleEmitter : public Entity {

	int max_particles = 600;
	int active_particles = 0;
	std::vector<sParticle> particles;

	// Properties of the emitter
	int last_id = 0;
	float emit_rate = 0.01f;
	float emit_timer = 0.f;

	Vector3 emit_position = {};
	Vector3 emit_velocity = {};
	float random_factor = 0.0f;

	// Properties of the particles
	float max_ttl = 3.f;
	std::vector<Vector4> colors = { Vector4(1.0f, 1.0f, 1.0f, 1.0f) };
	std::vector<float> sizes = { 0.5f };
	Texture* texture = nullptr;
	int texture_grid_size = 1;
	bool is_pixel_art = true;

	bool additive_blending = false;
	bool animated_texture = false;	// animation tiles
	bool sort_distance = true;
	bool emission_enabled = true;

	template<typename T>
	T sample(float time, int n, T* data) {
		// Convert range 0..1 to 0..n-1
		float time_in_table = time * (n - 1);
		// if time_in_table = 1.2.      entry = 1., amount = 0.2
		float entry;
		float amount = modf(time_in_table, &entry);
		return data[(int)entry] * (1 - amount) + data[(int)entry + 1] * amount;
	}

	void emit();

public:

	ParticleEmitter();
	~ParticleEmitter() = default;

	void render(Camera* camera) override;
	void update(float delta_time) override;

	void setTexture(const char* filename);
	void setTexture(Texture* new_texture) { texture = new_texture; }
	void setEmitPosition(const Vector3& position) { emit_position = position; }
	void setEmitVelocity(const Vector3& velocity) { emit_velocity = velocity; }
	void setEmitRate(float new_emit_rate) { emit_rate = new_emit_rate; }
	void setMaxTimeAlive(float new_ttl) { max_ttl = new_ttl; }
	void setEmissionEnabled(bool enabled) { emission_enabled = enabled; }
	void setAdditiveBlendingEnabled(bool enabled) { additive_blending = enabled; }
	void setAnimatedTexture(bool animated) { animated_texture = animated; }
	void setTextureGridSize(int new_texture_grid_size) { texture_grid_size = new_texture_grid_size; }
	void setMaxParticles(int new_max_particles) { max_particles = new_max_particles; }
	void setRandomFactor(float new_random_factor) { random_factor = new_random_factor; }
	void setColorsCurve(const std::vector<Vector4>& new_colors) { colors = new_colors; }
	void setSizesCurve(const std::vector<float>& new_sizes) { sizes = new_sizes; }

	Vector3 getEmitPosition() { return emit_position; }
	Vector3 getEmitVelocity() { return emit_velocity; }
	float getRandomFactor() { return random_factor; }
	float getMaxTimeAlive() { return max_ttl; }

	void clearParticles();
};
