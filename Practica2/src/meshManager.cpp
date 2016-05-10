#include <assert.h>
#include <sstream>
#include "includes.h"
#include "meshManager.h"

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

	std::stringstream ss;
	ss << filename << ".bin";
	std::string str = ss.str();
	const char* path = str.c_str();


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
