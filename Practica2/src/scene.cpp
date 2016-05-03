#include "scene.h"
#include <iostream>
#include <assert.h>
#include "extra/textparser.h"

//Source code for the Scene
Scene* Scene::instance = NULL;

Scene::Scene()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "Scene created" << std::endl;
	instance = this;
	root = new Entity();
}

Scene::~Scene()
{
	/*assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "MeshManager created" << std::endl;
	instance = this;*/
}

bool Scene::createLevel()
{
	//Creamos el nivel
	skybox = new EntityMesh();
	skybox->setup("data/meshes/space_cubemap/space_cubemap.ASE", "data/meshes/space_cubemap/nebula_cubemap.tga");
	skybox->global_matrix.scale(1000, 1000, 1000);
	skybox->frustum_text = false;
	//root->addEntity(skybox);

	planet = new EntityMesh();
	planet->setup("data/meshes/planet/sphere.ASE", "data/meshes/planet/planet_color.tga");
	planet->global_matrix.setTranslation(0, 0, -200);
	planet->global_matrix.scale(10, 10, 10);
	planet->frustum_text = false;
	planet->two_sided = true;
	root->addEntity(planet);

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