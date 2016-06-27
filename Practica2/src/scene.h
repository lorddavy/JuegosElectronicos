//#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "mesh.h"
//#include "entity.h"
#include <map>

class MeshManager;
class TextureManager;
class Entity;
class EntityMesh;
class EntityCollider;
class Vehicle;

class Scene {
public:

	MeshManager* meshManager;
	TextureManager* textureManager;

	Entity* root;
	EntityMesh* skybox;
	EntityCollider* planet;
	EntityCollider* station;
	Vehicle* runner;
	std::vector<Vehicle*> enemies;
	std::vector<EntityCollider*> asteroides;

	static Scene* getInstance() {
		if (instance == NULL)
			instance = new Scene();
		return instance;
	};

	bool createLevel();
	bool loadLevel(const char* filename);

	void clearRemovedEntities();
	//void addPlayer(const char* filename);
	Entity* createEntity(const char* name);
	bool checkPosition(EntityCollider* element);

	//std::vector<Entity*> postOrder();

private:
	static Scene* instance;
	Scene();
	~Scene();
};

#endif
