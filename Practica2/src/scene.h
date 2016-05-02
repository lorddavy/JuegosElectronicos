//#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "mesh.h"
#include "entity.h"
#include <map>

//Header For the Scene
class Scene {
public:
EntityMesh* skybox;
EntityMesh* planet;

Entity* root;

struct sSceneBin
{
	char format[5];
	int rootSize;
};

sSceneBin header;

static Scene* getInstance() {
	if (instance == NULL)
		instance = new Scene();
	return instance;
};

bool createLevel(const char * filename);
bool loadLevel(const char* filename);

private:
static Scene* instance;
Scene();
~Scene();
};

#endif
