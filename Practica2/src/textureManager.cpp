#include <assert.h>
#include "TextureManager.h"

//Source code for the Textures Manager
TextureManager* TextureManager::instance = NULL;

TextureManager::TextureManager()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "TextureManager created" << std::endl;
	instance = this;
}

Texture* TextureManager::getTexture(const char* path, const char* filename)
{
	auto it = s_map.find(filename);
	if (it != s_map.end())
		return it->second;

	std::string path_texture = path + std::string(filename);

	Texture* texture = new Texture();
	if (!texture->load(path_texture.c_str())) {
		std::cout << "texture not found: " << path_texture << std::endl;
		assert("FILE NOT FOUND!");
		delete texture;
		return NULL;
	}

	s_map[filename] = texture;
	return texture;

}
