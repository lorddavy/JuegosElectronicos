#include <iostream>
#include <assert.h>
#include <time.h>       /* time */

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
	skybox = NULL;
	planet = NULL;
	station = NULL;
	runner = NULL;
	enemies.clear();
	asteroides.clear();
}

Scene::~Scene()
{
}

bool Scene::createLevel()
{
	//Skybox
	if (skybox == NULL)
	{
		skybox = new EntityMesh();
		skybox->setup("data/cubemap/space_cubemap/", "space_cubemap.ASE", "stars.tga");
		//skybox->local_matrix.scale(300, 300, 300);
		//skybox->mesh->boundingBox.half_size = skybox->mesh->boundingBox.half_size * 300;
		skybox->frustum_text = false;
		//root->addEntity(skybox);
	}

	//Asteroides
	if (asteroides.size() == 0)
	{
		//Añadimos asteroides a la escena
		EntityCollider* element = (EntityCollider*)createEntity("grupoAsteroides");
		
		element->local_matrix.setTranslation(200, 200, 40);
		element->getGlobalMatrix();
		element->mesh->createCollisionModel();
		asteroides.push_back(element);
		//Lo agregamos a el vector de EntityCollider y al arbol de escena
		EntityCollider::static_entities.push_back(element);
		root->addEntity(element);

		int asteroidesSize = 3;

		//Más asteroides aleatorios
		for (int i = 0; i < asteroidesSize; i++) {
			if(i/3 < 1)	EntityCollider* element = (EntityCollider*)createEntity("asteroide1");
			else if(i/3 < 2) EntityCollider* element = (EntityCollider*)createEntity("asteroide2");
			else if(i/3 < 4) EntityCollider* element = (EntityCollider*)createEntity("asteroide3");

			srand(time(NULL));
			float randPosX = rand() % 300 - 50;
			float randPosY = rand() % 300 - 50;
			float randPosZ = rand() % 300 - 50;

			element->local_matrix.setTranslation(i * 10 + (randPosX), i * 10 + (randPosY), i * 10 + (randPosZ));
			Matrix44 globalMatrix = element->getGlobalMatrix();
			element->mesh->createCollisionModel();

			//Comprobamos si la posición es válida			
			Vector3 center = element->mesh->boundingBox.center;
			float radius = min(min(element->mesh->boundingBox.half_size.x, element->mesh->boundingBox.half_size.y), element->mesh->boundingBox.half_size.z);
			radius *= 1.2;
			bool valid = true;

			for (int i = 0; i < EntityCollider::static_entities.size(); ++i)
			{
				if (EntityCollider::static_entities[i]->mesh->testIntSphereMesh(EntityCollider::static_entities[i]->getGlobalMatrix(), globalMatrix * center, radius))
				{
					valid = false;
				}
			}

			if (valid)
			{
				asteroides.push_back(element);
				//Lo agregamos a el vector de EntityCollider y al arbol de escena
				EntityCollider::static_entities.push_back(element);
				root->addEntity(element);
			}
			else { delete element; }
		}
	}

	//Planeta
	if (planet == NULL)
	{
		planet = new EntityCollider();
		planet->setup("data/entityMesh/planet/", "sphere.ASE", "planet_color.tga");
		planet->local_matrix.setTranslation(0, 0, -200);
		planet->local_matrix.scale(10, 10, 10);
		planet->mesh->boundingBox.half_size = planet->mesh->boundingBox.half_size * 10;
		planet->global_matrix = planet->getGlobalMatrix();
		planet->frustum_text = false;
		planet->two_sided = true;
		planet->mesh->createCollisionModel();
		//Lo agregamos a el vector de EntityCollider y al arbol de escena
		EntityCollider::static_entities.push_back(planet);
		root->addEntity(planet);
	}

	//Estación espacial
	if (station == NULL)
	{
		station = new EntityCollider();
		station->setup("data/entityMesh/eve_station/", "eve_station.ASE", "eve2.tga");
		station->local_matrix.setTranslation(-800, 450, 0);
		station->local_matrix.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
		station->local_matrix.scale(0.5, 0.5, 0.5);
		station->mesh->boundingBox.half_size = station->mesh->boundingBox.half_size * 0.5;
		station->frustum_text = false;
		station->two_sided = true;
		station->mesh->createCollisionModel();
		//Lo agregamos a el vector de EntityCollider y al arbol de escena
		EntityCollider::static_entities.push_back(station);
		root->addEntity(station);
	}

	//Nave (runner)
	runner = (Vehicle*)createEntity("runner");
	runner->local_matrix.setTranslation(0, -10, 60);
	runner->local_matrix.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
	runner->mesh->createCollisionModel();
	//Lo agregamos a el vector de EntityCollider y al arbol de escena
	EntityCollider::dynamic_entities.push_back(runner);
	root->addEntity(runner);

	//Enemies
	/*if (enemies.size() == 0)
	{
		int enemiesSize = 1;
		for (int i = 0; i < enemiesSize; i++) {
			Vehicle* element = (Vehicle*)createEntity("spitfire");
			element->local_matrix.setTranslation(200, 0, 40);
			element->local_matrix.scale(1.5, 1.5, 1.5);
			element->mesh->boundingBox.half_size = element->mesh->boundingBox.half_size * 1.5;
			element->global_matrix = planet->getGlobalMatrix();
			element->mesh->createCollisionModel();
			enemies.push_back(element);
			//Lo agregamos a el vector de EntityCollider y al arbol de escena
			EntityCollider::dynamic_entities.push_back(element);
			root->addEntity(element);
		}
	}*/
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

void Scene::clearRemovedEntities() {
	//Borramos el contenedor con todo lo que se quiere destruir
	std::vector<Entity*>::iterator it;

	for (auto it = root->toDestroy.begin(); it != root->toDestroy.end(); ++it)
	{
		delete *it;
	}
	root->toDestroy.clear();
}

//Factory de entidades SCENE
Entity* Scene::createEntity(const char* type)
{
	Entity* entity = new Entity();
	std::string str = type;

	if (str == "runner")
	{
		Vehicle* vehicle = new Vehicle();
		vehicle->setup("data/vehicle/x3_runner/", "x3_runner.ASE", "x3_runner.tga");
		return vehicle;
	}
	else if (str == "spitfire")
	{
		Vehicle* vehicle = new Vehicle();
		vehicle->setup("data/vehicle/spitfire/", "spitfire.ASE", "spitfire_color_spec.tga", "spitfire_low.ASE");
		return vehicle;
	}
	else if (str == "grupoAsteroides")
	{
		EntityMesh* asteroide = new EntityMesh();
		asteroide->setup("data/entityMesh/asteroides/", "asteroides.ASE", "asteroide.tga");
		return asteroide;
	}
	else if (str == "asteroide1")
	{
		EntityMesh* asteroide = new EntityMesh();
		asteroide->setup("data/entityMesh/asteroides/", "asteroide1.ASE", "asteroide.tga");
		return asteroide;
	}
	else if (str == "asteroide2")
	{
		EntityMesh* asteroide = new EntityMesh();
		asteroide->setup("data/entityMesh/asteroides/", "asteroide2.ASE", "asteroide.tga");
		return asteroide;
	}
	else if (str == "asteroide3")
	{
		EntityMesh* asteroide = new EntityMesh();
		asteroide->setup("data/entityMesh/asteroides/", "asteroide3.ASE", "asteroide.tga");
		return asteroide;
	}
	return entity;
}
