#include <iostream>
#include <assert.h>

#include "scene.h"
#include "meshManager.h"
#include "textureManager.h"

#include "extra/textparser.h"

//Source code for the Scene
Scene* Scene::instance = NULL;

Scene::Scene()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "Scene created" << std::endl;
	instance = this;
	root = new Entity();

	MeshManager* meshManager = NULL;
	TextureManager* textureManager = NULL;


}

Scene::~Scene()
{
}

bool Scene::createLevel()
{
	//Skybox
	skybox = new EntityMesh();
	skybox->setup("data/meshes/space_cubemap/space_cubemap.ASE", "data/meshes/space_cubemap/nebula_cubemap.tga");
	skybox->local_matrix.scale(1000, 1000, 1000);
	skybox->frustum_text = false;
	//root->addEntity(skybox);

	//Planeta
	planet = new EntityMesh();
	planet->setup("data/meshes/planet/sphere.ASE", "data/meshes/planet/planet_color.tga");
	planet->local_matrix.setTranslation(0, 0, -200);
	planet->local_matrix.scale(10, 10, 10);
	planet->frustum_text = false;
	planet->two_sided = true;
	root->addEntity(planet);

	//Estación espacial
	station = new EntityMesh();
	station->setup("data/meshes/eve_station/eve_station.ASE", "data/meshes/eve_station/eve2.tga");
	station->local_matrix.setTranslation(-800, 450, 0);
	station->local_matrix.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
	station->local_matrix.scale(0.5, 0.5, 0.5);
	station->frustum_text = false;
	station->two_sided = true;
	root->addEntity(station);

	return true;
}

bool Scene::loadLevel(const char* filename)
{
	//Cargamos el nivel de un archivo
	TextParser t;

	if (FILE* f = fopen(filename, "rb"))
	{
		return true;
	}

	return false;
}