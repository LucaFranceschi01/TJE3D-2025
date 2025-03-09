#include "entity.h"

#include <algorithm>

#include "framework/includes.h"
#include "framework/camera.h"

void Entity::render(Camera* camera)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->render(camera);
	}
}

void Entity::update(float dt)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->update(dt);
	}
}

void Entity::addChild(Entity* child)
{
	if (child->parent) {
		std::cerr << "Child has already a parent, remove it first!" << std::endl;
		return;
	}

	// Checks if it's already a child
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end()) {
		std::cerr << "Entity is already one of the children!" << std::endl;
		return;
	}

	child->parent = this;
	children.push_back(child);
}

void Entity::removeChild(Entity* child)
{
	// Checks if it's a child
	auto it = std::find(children.begin(), children.end(), child);
	if (it == children.end()) {
		std::cerr << "Entity is not a child!!" << std::endl;
		return;
	}

	children.erase(it);
	child->parent = nullptr;
}

Matrix44 Entity::getGlobalMatrix() const
{
	if (parent)
		return model * parent->getGlobalMatrix();
	return model;
}

float Entity::distance(Entity* e) const
{
	return model.getTranslation().distance(e->model.getTranslation());
}

void Entity::onKeyDown(SDL_KeyboardEvent event)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->onKeyDown(event);
	}
}

void Entity::onKeyUp(SDL_KeyboardEvent event)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->onKeyUp(event);
	}
}

void Entity::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->onGamepadButtonDown(event);
	}
}

void Entity::onGamepadButtonUp(SDL_JoyButtonEvent event)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->onGamepadButtonUp(event);
	}
}
