//#pragma once
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "texture.h"

//Header For the Textures Manager
class TextureManager {
public:
std::map< std::string, Texture* > s_map;

static TextureManager* getInstance() {
	if (instance == NULL)
		instance = new TextureManager();
	return instance;
};

Texture* getTexture(const char* filename);

private:
static TextureManager* instance;
TextureManager();
};

#endif
