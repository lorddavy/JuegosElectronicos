#include "game.h"
#include "utils.h"
#include "scene.h"
#include "meshManager.h"
#include "mesh.h"
#include "textureManager.h"
#include "texture.h"
#include "rendertotexture.h"
#include "shader.h"
#include "vehicle.h"
#include "controller.h"
//#include <algorithm>

#include <cmath>

//some globals
Game* Game::instance = NULL;

Scene* scene = NULL;
MeshManager* meshmanager = NULL;
Mesh* mesh = NULL;
Mesh* mesh_low = NULL; //Low Quality Mesh
TextureManager* textureManager = NULL;
Texture* texture = NULL;

Controller* controller = NULL;

Shader* shader = NULL;
float angle = 0;
RenderToTexture* rt = NULL;

//Entidad del jugador
Vehicle* player;

//Camaras
//Camera* current_camera;
//Camera* free_camera;
//Camera* player_camera;
int cameraType = 0; //Camera type 0->free 1->player

Game::Game(SDL_Window* window)
{
	this->window = window;
	instance = this;

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	SDL_GetWindowSize( window, &window_width, &window_height );
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
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0,25,25),Vector3(0,0,0), Vector3(0,1,0)); //position the camera and point to 0,0,0
	camera->setPerspective(70,window_width/(float)window_height,0.1,10000); //set the projection, we want to be perspective

	scene = Scene::getInstance();
	meshmanager = MeshManager::getInstance();
	textureManager = TextureManager::getInstance();

	//free_camera = camera;

	//Carga de la escena
	if (!scene->loadLevel("data/scenes/space1.txt"))
	{
		scene->createLevel(); // dentor del loadLevel
	}

	//create a plane mesh
	/*shader = new Shader(); // Dentro de Entity
	if (!shader->load("data/shaders/simple.vs", "data/shaders/simple.fs"))
	{
		std::cout << "shader not found or error" << std::endl;
		exit(0);
	}*/

	//Creación de la entidad del jugador
	player = (Vehicle*)Game::createEntity("vehicle");
	player->local_matrix.setTranslation(0, 0, 0);
	player->local_matrix.rotate(180 * DEG2RAD, Vector3(0, 1, 0));

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
	drawText(5, 25, "Potencia de Impulso: 50%", Vector3(102/255, 255/255, 102/255), 2);

	glColor3f(1,1,1);
    
    glDisable( GL_BLEND );

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	scene->root->update(seconds_elapsed);

	//Camara libre
	if (cameraType == 0)
	{
		//mouse input to rotate the cam
		if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			camera->rotate(mouse_delta.x * 0.005, Vector3(0, -1, 0));
			camera->rotate(mouse_delta.y * 0.005, camera->getLocalVector(Vector3(-1, 0, 0)));
		}

		//async input to move the camera around
		if (keystate[SDL_SCANCODE_LSHIFT]) speed *= 10; //move faster with left shift
		if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) camera->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) camera->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) camera->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) camera->move(Vector3(-1, 0, 0) * speed);

	}

	//Camara jugador
	if (cameraType == 1)
	{

		/*Matrix44 global_player_matrix = player->getGlobalMatrix();
		Vector3 detras = global_player_matrix* Vector3(0, 2, -5);
		detras = camera->eye * 0.1 + detras *0.9;
		camera->lookAt(detras, global_player_matrix* Vector3(0, 0, 20), global_player_matrix.rotateVector(Vector3(0, 1, 0)));*/

		camera->lookAt(player->getGlobalMatrix() * Vector3(0, 2, -5), player->getGlobalMatrix() * Vector3(0, 0, 20), Vector3(0, 1, 0));

		//Control del jugador
		/*if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) camera->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) camera->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) camera->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) camera->move(Vector3(-1, 0, 0) * speed);*/

		//to navigate with the mouse fixed in the middle
		if (mouse_locked)
		{
			int center_x = floor(window_width*0.5);
			int center_y = floor(window_height*0.5);
			//center_x = center_y = 50;
			SDL_WarpMouseInWindow(this->window, center_x, center_y); //put the mouse back in the middle of the screen
																	 //SDL_WarpMouseGlobal(center_x, center_y); //put the mouse back in the middle of the screen

			this->mouse_position.x = center_x;
			this->mouse_position.y = center_y;
		}
	}	

	//Borramos el contenedor con todo lo que se quiere destruir
	while (scene->root->toDestroy.size() != 0) {
		Entity* e = scene->root->toDestroy.back();
		scene->root->toDestroy.pop_back();
		delete e;
	}
    
	//Rotación del planeta
	angle += seconds_elapsed * 10;
	scene->planet->local_matrix.rotateLocal(seconds_elapsed/50, Vector3(0, 1, 0));
}

//Keyboard event handler (sync input)
void Game::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
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


void Game::onMouseButton( SDL_MouseButtonEvent event )
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

	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
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
		vehicle->setup("data/meshes/spitfire/spitfire.ASE",
			"data/meshes/spitfire/spitfire_color_spec.tga",
			"data/meshes/spitfire/spitfire_low.ASE");
		vehicle->mesh->uploadToVRAM();
		vehicle->mesh_low->uploadToVRAM();
		//vehicle->local_matrix.setTranslation(0, 0, 0);
		return vehicle;
	}

	return entity;
}


