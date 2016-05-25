#include <cmath>
#include "utils.h"
#include "rendertotexture.h"

#include "game.h"
#include "scene.h"

#include "inputManager.h"
#include "shotManager.h"
//#include "collisionManager.h"

#include "vehicle.h"
#include "controller.h"

//free_camera, player_camera, current_camera !!

//some globals
Game* Game::instance = NULL;
Scene* scene = NULL;

//Managers
InputManager* inputManager = NULL;
ShotManager* shotManager = NULL;
//CollisionManager* collisionManager = NULL;

//std::vector<Vector3> debugPoints;
EntityMesh* debugEntityMesh = new EntityMesh();

Game::Game(SDL_Window* window)
{
	this->window = window;
	instance = this;

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	SDL_GetWindowSize(window, &window_width, &window_height);
	std::cout << " * Window size: " << window_width << " x " << window_height << std::endl;

	keystate = NULL;
	mouse_locked = false;
}

//Here we have already GL working, so we can create meshes and textures
void Game::init(void)
{
	std::cout << " * Path: " << getPath() << std::endl;

	//SDL_SetWindowSize(window, 50,50);

	//OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

	//Free camera
	free_camera = new Camera();
	free_camera->setPerspective(70, window_width / (float)window_height, 0.1, 10000);
	free_camera->lookAt(Vector3(0, 25, 25), Vector3(0, 0, 0), Vector3(0, 1, 0));
	current_camera = free_camera;

	scene = Scene::getInstance();

	//Managers
	scene->meshManager = MeshManager::getInstance();
	scene->textureManager = TextureManager::getInstance();
	inputManager = InputManager::getInstance();
	shotManager = ShotManager::getInstance();
	//collisionManager = CollisionManager::getInstance();

	//free_camera = camera;

	scene->loadLevel("data/scenes/space1.txt");

	player = scene->runner;

	//Player_camera
	player_camera = new Camera();
	player_camera->setPerspective(70, window_width / (float)window_height, 0.1, 10000);
	player_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 2, -5), player->getGlobalMatrix() * Vector3(0, 0, 20), Vector3(0, 1, 0));
	current_camera = player_camera;

	//Player controller
	controller.clear();
	controller.push_back(new Controller());
	controller[0]->setTarget(this->player);
	controller[0]->setCamera(player_camera);

	//Spitfire
	for (int i = 0; i < scene->spitfire.size(); i++) {
		Controller* element = new Controller();
		element->setTarget(scene->spitfire[i]);
		element->followTarget(player, Vector3(-30 + i * 15, 0, 0));
		controller.push_back(element);
	}
	/*spitfire_controller = new Controller();
	Vehicle* target = scene->spitfire.front();
	spitfire_controller->setTarget(target);*/

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse


	/*for (int i = 0; i < controller.size(); i++) {
		spitfire_controller[i]->followTarget(player, Vector3(-30 + i * 15, 0, 0));
	}*/

}

//what to do when the image has to be draw
void Game::render(void)
{
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	//Put the camera matrices on the stack of OpenGL (only for fixed rendering)
	current_camera->set();

	//Draw out world

	glDisable(GL_DEPTH_TEST);
	scene->skybox->local_matrix.setTranslation(current_camera->eye.x, current_camera->eye.y, current_camera->eye.z);
	scene->skybox->render(current_camera);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_DEPTH_BUFFER_BIT);

	drawGrid(500); //background grid

	scene->root->render(current_camera);
	shotManager->render(current_camera);

	//PROBANDO
	/*Mesh* debugMesh = new Mesh();	
	debugMesh->createQuad(300, 0, 100, 100, false);
	debugEntityMesh->mesh = debugMesh;
	debugEntityMesh->getGlobalMatrix();
	scene->root->addEntity(debugEntityMesh);*/

	//Dibujamos texto en pantalla
	drawText(5, 5, "Outer Space", Vector3(1, 0, 0), 3);
	
	std::string impulse = "Potencia de Impulso: " + player->getImpulse() + "%";
	drawText(5, 25, impulse, Vector3(102 / 255, 255 / 255, 102 / 255), 2);

	/*if (debugPoints.size())
	{
		glEnable(GL_BLEND);
		Mesh* debugMesh = new Mesh();
		
 		for (int i = 0; i < debugPoints.size(); i++)
		{
			debugMesh->createQuad(debugPoints[i].x, debugPoints[i].y, 10, 10, false);
			debugEntityMesh->mesh = debugMesh;
			debugEntityMesh->color = (1, 0, 0);
			scene->root->addEntity(debugEntityMesh);
		}

	}*/

	glColor3f(1, 1, 1);

	glDisable(GL_BLEND);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	//Updates
	scene->root->update(seconds_elapsed);
	inputManager->update(seconds_elapsed);
	shotManager->update(seconds_elapsed*time);

	//Borramos el contenedor con todo lo que se quiere destruir
	scene->clearRemovedEntities();

	//Comprobamos colisiones
	/*for (int i = 0; i < 50; i++)
	{
		Shot& shot = shotManager->shots[i];
		
		if (shot.active) {
			if (scene->planet->mesh->testIntRayMesh(scene->planet->getGlobalMatrix(), player->getGlobalMatrix() * shot.origin_position, player->getGlobalMatrix() * (shot.end_position - shot.origin_position)))
			{
				float collisionPoint[3];
				scene->planet->mesh->collision_model->getCollisionPoint(collisionPoint, true);
				float t1[9], t2[9];
				scene->planet->mesh->collision_model->getCollidingTriangles(t1, t2, false);

				//debugPoints.push_back(Vector3(collisionPoint[0], collisionPoint[1], collisionPoint[2]));
				std::cout << "Colisi�n en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
			}
		}

		float radius = player->mesh->boundingBox.half_size.z;
		if (scene->planet->mesh->testIntSphereMesh(scene->planet->getGlobalMatrix(), player->getGlobalMatrix() * player->mesh->boundingBox.center, radius))
		{
			std::cout << "Colision" << std::endl;
		}
		//std::cout << (player->getGlobalMatrix() * player->mesh->boundingBox.center).x << std::endl;
	}*/

	//Rotaci�n del planeta
	scene->planet->local_matrix.rotateLocal(seconds_elapsed / 50, Vector3(0, 1, 0));

	//Spitfire
	//scene->spitfire->velocity = (player->getGlobalMatrix() * Vector3(0, 0, 0)).normalize();
	//scene->spitfire->velocity = Vector3(-1, 1000*seconds_elapsed, 0);
	
	//scene->spitfire->pitch(0.01);	

}

//Keyboard event handler (sync input)
void Game::onKeyPressed(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: exit(0); //ESC key, kill the app
	case SDLK_TAB: //Change free camera-player camera
		if (current_camera == free_camera)
			current_camera = player_camera;
		else if (current_camera == player_camera){
			free_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 15, -35),
				player->getGlobalMatrix() * Vector3(0, 0, 20),
				player->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)));
			current_camera = free_camera;
		}
	}
}


void Game::onMouseButton(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::setWindowSize(int width, int height)
{
	std::cout << "window resized: " << width << "," << height << std::endl;

	/*
	Uint32 flags = SDL_GetWindowFlags(window);
	if(flags & SDL_WINDOW_ALLOW_HIGHDPI)
	{
	width *= 2;
	height *= 2;
	}
	*/

	glViewport(0, 0, width, height);
	current_camera->aspect = width / (float)height;
	window_width = width;
	window_height = height;
}


//Factory de entidades
/*Entity* Game::createEntity(const char* name)
{
	Entity* entity = new Entity();
	std::string str = name;

	if (str == "vehicle")
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
*/

