#include "scene.h"
#include <iostream>
#include <assert.h>

//Source code for the Scene
Scene* Scene::instance = NULL;

Scene::Scene()
{
assert(instance == NULL && "must be only one"); //must be only one, en release no petará
std::cout << "Scene created" << std::endl;
instance = this;
}

Scene::~Scene()
{
	/*assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "MeshManager created" << std::endl;
	instance = this;*/
}

void Scene::createLevel(const char* filename)
{
	//Creamos el nivel
	//Entity* skybox_root = new Entity();
	//EntityMesh* skybox = new EntityMesh();
	/*skybox->setup("data/meshes/cubemap_paisaje/cubemap.ASE", "data/meshes/cubemap_paisaje/cielo.tga");
	skybox->local_matrix.setScale(100, 100, 100);
	skybox->frustum_text = false;*/
	//root->addEntity(skybox);
}
