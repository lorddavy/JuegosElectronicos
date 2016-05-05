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
//#include <algorithm>

#include <cmath>

//some globals
Game* Game::instance = NULL;

Scene* scene = Scene::getInstance();

MeshManager* meshmanager = MeshManager::getInstance();
Mesh* mesh = NULL;
Mesh* mesh_low = NULL; //Low Quality Mesh
TextureManager* textureManager = TextureManager::getInstance();
Texture* texture = NULL;
Shader* shader = NULL;
float angle = 0;
RenderToTexture* rt = NULL;

//Entidad del jugador
Vehicle* player;

//Camaras
Camera* current_camera;
Camera* free_camera;
Camera* player_camera;

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


	//free_camera = camera;

	//Carga de la escena
	if (!scene->loadLevel("data/scenes/space1.txt"))
	{
		scene->createLevel();
	}

	//Carga de las mallas de los objetos

		/*for (int i = -10; i < 10; i++)
		{
			for (int j = -10; j < 10; j++)
			{
				EntityMesh* spitfire = new EntityMesh();
				EntityMesh* prev_entity = NULL;
				spitfire->setup("data/meshes/spitfire/spitfire.ASE", "data/meshes/spitfire/spitfire_color_spec.tga", "data/meshes/spitfire/spitfire_low.ASE");
				spitfire->mesh->uploadToVRAM();
				spitfire->mesh_low->uploadToVRAM();
				spitfire->local_matrix.setTranslation(i * 10, j * 10, 0);

				if (prev_entity)
					prev_entity->addEntity(spitfire);
				else
					scene->root->addEntity(spitfire);
				//header.rootSize += 1;
				prev_entity = spitfire;
			}
		}*/

	//create a plane mesh
	shader = new Shader();
	if (!shader->load("data/shaders/simple.vs", "data/shaders/simple.fs"))
	{
		std::cout << "shader not found or error" << std::endl;
		exit(0);
	}

	//Creación de la entidad del jugador
	player = (Vehicle*)Game::createEntity("vehicle");
	player->local_matrix.setTranslation(0, 0, 0);
	player->local_matrix.rotate(180 * DEG2RAD, Vector3(0, 1, 0));

	scene->root->addEntity(player);

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
	drawGrid(500); //background grid
    
	//create model matrix from plane
	Matrix44 m;
    m.setScale(1,1,1);
	m.rotate(angle * DEG2RAD, Vector3(0,1,0) ); //build a rotation matrix

	//draw the plane
	if(0) //render using shader
	{
		/*for (int i = -100; i < 100; i++)
		{
			for (int j = -100; j < 100; j++)
			{
				m.setTranslation(i * 10, j * 10, 0); //Para hacer benchmarking pintamos 100 aviones
				Matrix44 mvp = m * camera->viewprojection_matrix;

				if (Vector3(i * 10, 0, 0).distance(camera->eye) > 100) //Escogemos la textura que toca
				{
					render_mesh = mesh_low;
				}
				else { render_mesh = mesh; }

				float size = max(max(render_mesh->boundingBox.half_size.x, render_mesh->boundingBox.half_size.y), render_mesh->boundingBox.half_size.z);

				if (camera->testSphereInFrustum(Vector3(i * 10, 0, 0), size) == true)
				{
					shader->enable();
					shader->setMatrix44("u_model", m);
					shader->setMatrix44("u_mvp", mvp);
					render_mesh->render(GL_TRIANGLES, shader);
					shader->disable();
				}
			}
		}	*/	
	}
	else //render using fixed pipeline (DEPRECATED)
	{
		glDisable(GL_DEPTH_TEST);
		scene->skybox->local_matrix.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
			scene->skybox->render(camera);
		glEnable(GL_DEPTH_TEST);

		glClear(GL_DEPTH_BUFFER_BIT);

		scene->root->render(camera);

		/*Camera cam;
		cam = *camera;
		cam.lookAt(Vector3(0, 0, 0), camera->center - camera->eye, Vector3(0, 1, 0));
		cam.set();
		scene->planet->render(&cam);
		camera->set();*/		
	}

	//Dibujamos texto en pantalla
	drawText(5, 5, "Outer Space", Vector3(1, 0, 0), 3);
    
    glDisable( GL_BLEND );

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	//Controles de cámara libre
	//if (current_camera == free_camera)
	//{
		

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
	//}

	//if (current_camera == player_camera)
	//{

		/*Matrix44 global_player_matrix = player->getGlobalMatrix();
		Vector3 detras = global_player_matrix* Vector3(0, 2, -5);
		detras = camera->eye * 0.1 + detras *0.9;
		camera->lookAt(detras, global_player_matrix* Vector3(0, 0, 20), global_player_matrix.rotateVector(Vector3(0, 1, 0)));*/

		player->update(seconds_elapsed);
		//Control del jugador
		/*if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) camera->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) camera->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) camera->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) camera->move(Vector3(-1, 0, 0) * speed);*/
	//}

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
		//case SDLK_TAB: //current_camera = free_camera; //Change to free camera
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


