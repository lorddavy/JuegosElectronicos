//This code works
#include <cmath>
#include "utils.h"
#include "rendertotexture.h"

#include "game.h"
#include "scene.h"

#include "inputManager.h"
#include "shotManager.h"

#include "vehicle.h"

//some globals
Game* Game::instance = NULL;
Scene* scene = NULL;

//Managers
InputManager* inputManager = NULL;
ShotManager* shotManager = NULL;

//std::vector<Vector3> debugPoints;

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

	camera = new Camera(); //create our camera
	camera->lookAt(Vector3(0, 25, 25), Vector3(0, 0, 0), Vector3(0, 1, 0)); //position the camera and point to 0,0,0
	camera->setPerspective(70, window_width / (float)window_height, 0.1, 10000); //set the projection, we want to be perspective
	cameraType = 0; //Camera type 0->free 1->player

	scene = Scene::getInstance();

	//Managers
	scene->meshManager = MeshManager::getInstance();
	scene->textureManager = TextureManager::getInstance();
	inputManager = InputManager::getInstance();
	shotManager = ShotManager::getInstance();

	//free_camera = camera;

	scene->loadLevel("data/scenes/space1.txt");

	player = scene->runner;

	//scene->addPlayer("data/scenes/player.txt");
	/*
	player = (Vehicle*) this->createEntity("runner");
	player->local_matrix.setTranslation(0, 0, 40);
	player->local_matrix.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
	
	scene->root->addEntity(player);
	*/
	cameraType = 1;

	//Camera* player_camera = new Camera();
	//player_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 2, -5), player->getGlobalMatrix() * Vector3(0, 0, 20), Vector3(0, 1, 0));

	//current_camera = player_camera;
	//camera->lookAt(player->getGlobalMatrix() * Vector3(0, 2, -5), player->getGlobalMatrix() * Vector3(0, 0, 20), Vector3(0, 1, 0));

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
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
	camera->set();

	//Draw out world

	glDisable(GL_DEPTH_TEST);
	scene->skybox->local_matrix.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
	scene->skybox->render(camera);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_DEPTH_BUFFER_BIT);

	drawGrid(500); //background grid

	scene->root->render(camera);
	shotManager->render(camera);

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
			debugMesh->vertices.push_back(Vector3(debugPoints[i].x, debugPoints[i].y, debugPoints[i].z));
			debugMesh->colors.push_back(Vector4(1, 0, 0, 0));
		}

		debugMesh->render(1);

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
	for (int i = 0; i < 50; i++)
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
				std::cout << "Colisión en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
			}
		}

		float radius = player->mesh->boundingBox.half_size.z;
		if (scene->planet->mesh->testIntSphereMesh(scene->planet->getGlobalMatrix(), player->getGlobalMatrix() * player->mesh->boundingBox.center, radius))
		{
			std::cout << "Colision" << std::endl;
		}
		//std::cout << (player->getGlobalMatrix() * player->mesh->boundingBox.center).x << std::endl;
	}

	//Rotación del planeta
	scene->planet->local_matrix.rotateLocal(seconds_elapsed / 50, Vector3(0, 1, 0));
}

//Keyboard event handler (sync input)
void Game::onKeyPressed(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); //ESC key, kill the app
		case SDLK_TAB: //Change free camera-player camera
			if (cameraType == 1)
			{
				cameraType = 0;
				return;
			}
			if (cameraType == 0)
			{
				cameraType = 1;
				return;
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
	camera->aspect = width / (float)height;
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

