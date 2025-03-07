#include "scene_parser.h"

#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"

#include "framework/utils.h"
#include "framework/entities/entityCollider.h"

#include <fstream>

bool SceneParser::parse(const char* filename, Entity* root)
{
	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		if (mesh_name[0] == '#')
			continue;

		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}

	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, nothing to do here
		if (render_data.models.empty())
			continue;

		Material mat = render_data.material;
		EntityCollider* new_entity = nullptr;

		size_t tag = data.first.find("@Tag"); // TODO: use parser tags

		Mesh* mesh = Mesh::Get(mesh_name.c_str());
		new_entity = new EntityCollider(mesh, mat);


		if (tag != std::string::npos) {

			if (data.first.find("@Tag.Ground") != std::string::npos) {
				new_entity->layer = GROUND;
			} else if (data.first.find("@Tag.Fluid") != std::string::npos) {
				new_entity->layer = FLUID;
			} else if (data.first.find("@Tag.Pin") != std::string::npos) {
				new_entity->layer = PIN;
				new_entity->material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
			} else if (data.first.find("@Tag.Booster") != std::string::npos) {
				new_entity->layer = BOOSTER;
				new_entity->material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
			}

		} else {
			//Mesh* mesh = Mesh::Get(mesh_name.c_str());
			//new_entity = new EntityCollider(mesh, mat);
			new_entity->layer = OBSTACLE;
		}

		if (!new_entity) {
			continue;
		}

		new_entity->name = data.first;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			new_entity->isInstanced = true;
			//new_entity->models = render_data.models; // Add all instances
			for (auto model : render_data.models) {
				new_entity->models.emplace_back(model, true);

			}
		}
		// Create normal entity
		else {
			new_entity->model = render_data.models[0];
		}

		// Add entity to scene root
		root->addChild(new_entity);
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}