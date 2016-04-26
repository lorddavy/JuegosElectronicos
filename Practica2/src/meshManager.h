//#pragma once
#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include "mesh.h"
#include <map>

//Header For the Textures Manager
class MeshManager {
public:
std::map< std::string, Mesh* > s_map;

static MeshManager* getInstance() {
	if (instance == NULL)
		instance = new MeshManager();
	return instance;
};

Mesh* getMesh(const char* filename);

private:
static MeshManager* instance;
MeshManager();
};

#endif
