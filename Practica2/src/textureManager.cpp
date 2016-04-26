#include "TextureManager.h"
#include <assert.h>

//Source code for the Textures Manager
TextureManager* TextureManager::instance = NULL;

TextureManager::TextureManager()
{
assert(instance == NULL && "must be only one"); //must be only one, en release no petará
std::cout << "TextureManager created" << std::endl;
instance = this;
}

Texture* TextureManager::getTexture(const char* filename)
{
	auto it = s_map.find(filename);
	if (it != s_map.end())
		return it->second;

	Texture* texture = new Texture();
	if (!texture->load(filename)) {
		std::cout << "texture not found" << filename << std::endl;
		assert("FILE NOT FOUND!");
		delete texture;
		return NULL;
	}

	s_map[filename] = texture;
	return texture;

}
