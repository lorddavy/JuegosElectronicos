#include "meshManager.h"
#include <iostream>
#include <assert.h>

//Source code for the Textures Manager
MeshManager* MeshManager::instance = NULL;

MeshManager::MeshManager()
{
assert(instance == NULL && "must be only one"); //must be only one, en release no petará
std::cout << "TextureManager created" << std::endl;
instance = this;
}

Mesh* MeshManager::getMesh(const char* filename)
{
	auto it = s_map.find(filename);
	if (it != s_map.end())
		return it->second;

	Mesh* mesh = new Mesh();
	if (!mesh->loadASE(filename)) {
		std::cout << "texture not found" << filename << std::endl;
		assert("FILE NOT FOUND!");
		delete mesh;
		return NULL;
	}

	s_map[filename] = mesh;
	return mesh;

}
