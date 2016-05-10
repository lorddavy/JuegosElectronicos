#include <iostream>
#include <assert.h>

#include "scene.h"
#include "meshManager.h"
#include "textureManager.h"
#include "inputManager.h"

#include "entity.h"
#include "vehicle.h"

#include "extra/textparser.h"

//Source code for the Scene
Scene* Scene::instance = NULL;

Scene::Scene()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "Scene created" << std::endl;
	instance = this;
	root = new Entity();

	meshManager = NULL;
	textureManager = NULL;
	player = NULL;
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
	//TextParser t;

	std::stringstream ss;

	if (FILE* f = fopen(filename, "rb")) {
		ss << "Level loaded: " << filename;
		std::cout << ss.str() << std::endl;
	}
	else {
		createLevel();
		ss << "Level " << filename << " doesn't exist!";
		std::cout << ss.str() << std::endl;
	}
	
	return false;
}

void Scene::clearEntities() {
	//Borramos el contenedor con todo lo que se quiere destruir
	while (root->toDestroy.size() != 0) {
		Entity* e = root->toDestroy.back();
		root->toDestroy.pop_back();
		delete e;
	}
}

void Scene::addPlayer(const char* type) {
	//Creación de la entidad del jugador					Seguro que no se tiene que crear en scene???
	
	player = (Vehicle*) this->createEntity(type);
	player->local_matrix.setTranslation(0, 0, 40);
	player->local_matrix.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
	root->addEntity(player);
}

//Factory de entidades SCENEE
Entity* Scene::createEntity(const char* type)
{
	Entity* entity = new Entity();
	std::string str = type;

	if (str == "runner")
	{
		Vehicle* vehicle = new Vehicle();
		//creamos spitfire
		vehicle->setup("data/meshes/x3_runner/x3_runner.ASE",
			"data/meshes/x3_runner/x3_runner.tga");
		//vehicle->mesh->uploadToVRAM();
		//vehicle->mesh_low->uploadToVRAM();
		return vehicle;
	}

	return entity;
}
