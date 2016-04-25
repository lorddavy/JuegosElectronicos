#include "game.h"
#include "utils.h"
#include "meshManager.h"
#include "mesh.h"
#include "textureManager.h"
#include "texture.h"
#include "rendertotexture.h"
#include "shader.h"
//#include <algorithm>

#include <cmath>

//some globals
MeshManager* meshmanager = MeshManager::getInstance();
Mesh* mesh = NULL;
Mesh* mesh_low = NULL; //Low Quality Mesh
TextureManager* textureManager = TextureManager::getInstance();
Texture* texture = NULL;
Shader* shader = NULL;
float angle = 0;
RenderToTexture* rt = NULL;

Game* Game::instance = NULL;

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

	//create a plane mesh
	mesh = new Mesh();
	mesh_low = new Mesh();

	shader = new Shader();
	if (!shader->load("data/shaders/simple.vs", "data/shaders/simple.fs"))
	{
		std::cout << "shader not found or error" << std::endl;
		exit(0);
	}

	//Carga de las mallas de los objetos

	mesh = MeshManager::getInstance()->getMesh("data/meshes/spitfire/spitfire.ASE");
	mesh_low = MeshManager::getInstance()->getMesh("data/meshes/spitfire/spitfire_low.ASE");

	//Desde aqui -> Todo este codigo dentro del mesh Manager
	/*if (mesh->readBIN("data/meshes/spitfire/spitfire.ASE.bin") == false)
	{
		if (mesh->loadASE("data/meshes/spitfire/spitfire.ASE") == false)
		{
			std::cout << "file not found" << std::endl;
			exit(0);
		}
	}

	if (mesh_low->readBIN("data/meshes/spitfire/spitfire_low.ASE.bin") == false)
	{
		if (mesh_low->loadASE("data/meshes/spitfire/spitfire_low.ASE") == false)
		{
			std::cout << "file not found" << std::endl;
			exit(0);
	}*/
	//Hasta aqui -> Todo este codigo dentro del mesh Manager


	//Las mallas las subimos a la GPU para que sea más eficiente el renderizado
	mesh->uploadToVRAM();
	mesh_low->uploadToVRAM();

	//Cargamos Texture. Eso tendra que ir dentro del mesh manager
	texture = TextureManager::getInstance()->getTexture("data/meshes/spitfire/spitfire_color_spec.tga");

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{
	Mesh* render_mesh = NULL; //Tendremos que quitarlo!!!

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
		//for (int i = -25; i < 25; i++)
		for (int i = 0; i < 1; i++)
		{
			m.setTranslation(i * 10, 0, 0); //Para hacer benchmarking pintamos 100 aviones
			Matrix44 mvp = m * camera->viewprojection_matrix;			

			if (Vector3(i * 10, 0, 0).distance(camera->eye) > 100) //Escogemos la textura que toca
			{render_mesh = mesh_low;}else{render_mesh = mesh;}

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
		
	}
	else //render using fixed pipeline (DEPRECATED)
	{

		for (int i = -100; i < 100; i++)
		{
			for (int j = -100; j < 100; j++)
			{
				m.setTranslation(i * 10, j * 10, 0); //Para hacer benchmarking pintamos 100 aviones
				Matrix44 mvp = m * camera->viewprojection_matrix;

				if (Vector3(i * 10, j * 10, 0).distance(camera->eye) > 100) //Escogemos la mesh que toca segï¿½n distancia
					render_mesh = mesh_low;
				else
					render_mesh = mesh;

				//Hacemos frustum culling para pintar solo lo que hay en frustum
				float size = max(max(render_mesh->boundingBox.half_size.x, render_mesh->boundingBox.half_size.y), render_mesh->boundingBox.half_size.z);
				if (camera->testSphereInFrustum(Vector3(i * 10, j * 10, 0), size) == true)
				{
					glPushMatrix();
					m.multGL();
					texture->bind();
					render_mesh->render(GL_TRIANGLES);
					texture->unbind();
					glPopMatrix();
				}

			}
		}
	}

	//Dibujamos texto en pantalla
	//drawText(0, 0, "Numero de aviones: 40000", Vector3(0, 0, 0), 4);
    
    glDisable( GL_BLEND );

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	double speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	//mouse input to rotate the cam
	if ((mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
		camera->rotate(mouse_delta.x * 0.005, Vector3(0,-1,0));
		camera->rotate(mouse_delta.y * 0.005, camera->getLocalVector( Vector3(-1,0,0)));
	}

	//async input to move the camera around
	if(keystate[SDL_SCANCODE_LSHIFT]) speed *= 10; //move faster with left shift
	if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) camera->move(Vector3(0,0,1) * speed);
	if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) camera->move(Vector3(0,0,-1) * speed);
	if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) camera->move(Vector3(1,0,0) * speed);
	if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) camera->move(Vector3(-1,0,0) * speed);
    
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
    

	angle += seconds_elapsed * 10;
}

//Keyboard event handler (sync input)
void Game::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); //ESC key, kill the app
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

