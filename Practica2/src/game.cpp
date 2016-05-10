#include <cmath>

#include "game.h"

#include "scene.h"
#include "meshManager.h"
#include "textureManager.h"
#include "inputManager.h"

#include "utils.h"
#include "rendertotexture.h"

#include "vehicle.h"



//some globals
Game* Game::instance = NULL;
Scene* scene = NULL;

//dentro de scene
MeshManager* meshManager = NULL;
TextureManager* textureManager = NULL;

InputManager* inputManager = NULL;	//dentro de scene???? PROBLEMAS!!


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
	meshManager = MeshManager::getInstance();
	textureManager = TextureManager::getInstance();

	//free_camera = camera;

	//Carga de la escena
	if (!scene->loadLevel("data/scenes/space1.txt"))
	{
		scene->createLevel(); // dentro del loadLevel
	}

	//Creación de la entidad del jugador
	player = (Vehicle*)Game::createEntity("vehicle");
	player->local_matrix.setTranslation(0, 0, 40);
	player->local_matrix.rotate(270 * DEG2RAD, Vector3(0, 1, 0));

	scene->root->addEntity(player);

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

	//Dibujamos texto en pantalla
	drawText(5, 5, "Outer Space", Vector3(1, 0, 0), 3);
	
	std::string impulse = "Potencia de Impulso: " + player->getImpulse() + "%";
	drawText(5, 25, impulse, Vector3(102 / 255, 255 / 255, 102 / 255), 2);

	glColor3f(1, 1, 1);

	glDisable(GL_BLEND);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	scene->root->update(seconds_elapsed);

	inputManager->update(seconds_elapsed);

	//Borramos el contenedor con todo lo que se quiere destruir
	while (scene->root->toDestroy.size() != 0) {
		Entity* e = scene->root->toDestroy.back();
		scene->root->toDestroy.pop_back();
		delete e;
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
Entity* Game::createEntity(const char* name)
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


