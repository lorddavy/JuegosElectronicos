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

Mesh* MeshManager::getMesh(const char* path, const char* filename)
{
	auto it = s_map.find(filename);
	if (it != s_map.end())
		return it->second;

	/*std::stringstream ss;
	ss << filename << ".bin";
	std::string str = ss.str();
	const char* path = str.c_str();*/

	std::string path_bin = std::string("data/bin/") + filename + std::string(".bin");
	std::string path_mesh = path + std::string(filename);

	Mesh* mesh = new Mesh();
	if (mesh->readBIN(path_bin.c_str()) == false)
	{
		if (mesh->loadASE(path_mesh.c_str(), path_bin.c_str()) == false)
		{
			std::cout << "mesh not found: " << path_mesh << std::endl;
			assert("FILE NOT FOUND!");
			delete mesh;
			return NULL;
		}
	}

	s_map[filename] = mesh;
	return mesh;

}
