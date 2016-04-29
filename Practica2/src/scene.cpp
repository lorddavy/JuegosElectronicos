#include "scene.h"
#include <iostream>
#include <assert.h>

//Source code for the Scene
Scene* Scene::instance = NULL;

Scene::Scene()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petar�
	std::cout << "Scene created" << std::endl;
	instance = this;
	root = new Entity();
}

Scene::~Scene()
{
	/*assert(instance == NULL && "must be only one"); //must be only one, en release no petar�
	std::cout << "MeshManager created" << std::endl;
	instance = this;*/
}

void Scene::createLevel(const char* filename)
{
	//Creamos el nivel
	//skybox = new EntityMesh();
	//skybox->setup("data/meshes/space_cubemap/space_cubemap.ASE", "data/meshes/space_cubemap/nebula_cubemap.tga");
	//skybox->global_matrix.setScale(100, 100, 100);
	//skybox->frustum_text = false;
	//root->addEntity(skybox);

	//EntityMesh* planet = new EntityMesh();
	//planet->setup("data/meshes/planet/sphere.ASE", "data/meshes/planet/planet_color.tga");
	//planet->global_matrix.setTranslation(0, 0, -50);
	//planet->global_matrix.scale(10, 10, 10);
	//planet->frustum_text = false;
	//planet->two_sided = true;
	//root->addEntity(planet);
}
