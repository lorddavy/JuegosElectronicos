//#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "mesh.h"
#include "entity.h"
#include <map>

//Header For the Scene
class Scene {
public:
Entity* root;

static Scene* getInstance() {
	if (instance == NULL)
		instance = new Scene();
	return instance;
};

void createLevel(const char* filename);

private:
static Scene* instance;
Scene();
~Scene();
};

#endif
