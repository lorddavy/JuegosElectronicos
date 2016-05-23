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
class Vehicle;

class Scene {
public:

	MeshManager* meshManager;
	TextureManager* textureManager;

	Entity* root;
	EntityMesh* skybox;
	EntityMesh* planet;
	EntityMesh* station;
	Vehicle* runner;

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


private:
	static Scene* instance;
	Scene();
	~Scene();
};

class CollisionManager {
public:
	std::vector<EntityMesh*> static_entities;
	std::vector<EntityMesh*> dynamic_entities;

	static CollisionManager* getInstance() {
		if (instance == NULL)
			instance = new CollisionManager();
		return instance;
	};

	void update(float dt);

private:
	static CollisionManager* instance;
	CollisionManager();

};

#endif
