#include "meshManager.h"
#include <iostream>
#include <assert.h>

//Source code for the Textures Manager
MeshManager* MeshManager::instance = NULL;

MeshManager::MeshManager()
{
assert(instance == NULL && "must be only one"); //must be only one, en release no petará
std::cout << "MeshManager created" << std::endl;
instance = this;
}

Mesh* MeshManager::getMesh(const char* filename)
{
	auto it = s_map.find(filename);
	if (it != s_map.end())
		return it->second;

	char path[100];
	strcpy(path, filename);
	strcat(path, ".bin");

	Mesh* mesh = new Mesh();
	if (mesh->readBIN(path) == false)
	{
		if (mesh->loadASE(filename) == false)
		{
			std::cout << "mesh not found" << std::endl;
			assert("FILE NOT FOUND!");
			delete mesh;
			return NULL;
		}
	}

	s_map[filename] = mesh;
	return mesh;

}
