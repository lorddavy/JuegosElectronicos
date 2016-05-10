//#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "mesh.h"
#include "entity.h"
#include <map>

class MeshManager;
class TextureManager;

class Scene {
public:

	MeshManager* meshManager;
	TextureManager* textureManager;

	Entity* root;
	EntityMesh* skybox;
	EntityMesh* planet;
	EntityMesh* station;

	static Scene* getInstance() {
		if (instance == NULL)
			instance = new Scene();
		return instance;
	};

	bool createLevel();
	bool loadLevel(const char* filename);


private:

	static Scene* instance;
	Scene();
	~Scene();
};

#endif
