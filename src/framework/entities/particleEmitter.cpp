#include "ParticleEmitter.h"

void ParticleEmitter::emit()
{
	emit_timer = 0.f;

	for (s_Particle& particle : particles) {
		if (particle.active) continue;

		particle.id = last_id++;
		particle.ttl = 0.f;

	}
}

ParticleEmitter::ParticleEmitter()
{
}

void ParticleEmitter::render(Camera* camera)
{
}

void ParticleEmitter::update(float dt)
{
	emit_timer += dt;
	if (emission_enabled && emit_timer > emit_rate && active_particles < max_particles)
	{
		emit();
	}

	/*for (s_Particle& particle : particles) {
		if (!particle.active) continue;

		Vector3 random_vel = Vector3(random(random_factor, -random_factor*0.5f))
	}*/
}

void ParticleEmitter::setTexture(const char* filename)
{
}

void ParticleEmitter::clearParticles()
{
	particles.clear();
	particles.resize(max_particles);
	emit_timer = 0.f;
	active_particles = 0;
}
