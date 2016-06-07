#include <cmath>
#include "utils.h"
#include "rendertotexture.h"

#include "game.h"
#include "scene.h"

#include "inputManager.h"
#include "shotManager.h"
#include "collisionManager.h"

#include "vehicle.h"
#include "controller.h"

//free_camera, player_camera, current_camera !!

//some globals
Game* Game::instance = NULL;
Scene* scene = NULL;

//Managers
InputManager* inputManager = NULL;
ShotManager* shotManager = NULL;
CollisionManager* collisionManager = NULL;

//DEBUG
//std::vector<Vector3> debugPoints;
EntityCollider* debugEntityMesh;
EntityMesh* testMesh;

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
//Inicio del juego
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
	//current_camera = free_camera;

	//Etapa inicial
	currentStage = "";

	scene = Scene::getInstance();

	//Managers
	scene->meshManager = MeshManager::getInstance();
	scene->textureManager = TextureManager::getInstance();
	inputManager = InputManager::getInstance();
	shotManager = ShotManager::getInstance();
	collisionManager = CollisionManager::getInstance();

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
	controller.push_back(new Controller(false));
	controller[0]->setTarget(this->player);
	controller[0]->setCamera(player_camera);	
	player->controller = controller[0];//Puntero a su controlador

	//Spitfire
	for (int i = 0; i < scene->spitfire.size(); i++) {
		Controller* element = new Controller();
		element->setTarget(scene->spitfire[i]);
		//element->followTarget(player, Vector3(-30 + i * 15, 0, 0));
		controller.push_back(element);		
		scene->spitfire[i]->controller = element;//Puntero a su controlador
	}

	//DEBUG MESH (COLLISIONS)
	/*debugEntityMesh = new EntityCollider();

	Mesh* debugMesh = new Mesh();
	debugMesh->createQuad(300, 0, 100, 100, false);
	debugMesh->createCollisionModel();
	debugEntityMesh->mesh = debugMesh;
	debugEntityMesh->getGlobalMatrix();
	debugEntityMesh->static_entities.push_back(debugEntityMesh);
	scene->root->addEntity(debugEntityMesh);*/

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	// Test function loadOBJ
	/*testMesh = new EntityMesh();
	testMesh->mesh->loadOBJ("data/objtest/test.obj", "data/bin/");
	scene->root->addEntity(testMesh);*/
}

//Final de juego
void Game::end(void)
{
	//Acabamos con lo que no necesitamos
	current_camera = free_camera;	
	currentStage = "defeat";
}

//Reinicio del juego
void Game::restart(void)
{
	//Seteamos a los valores iniciales

	/*free_camera->setPerspective(70, window_width / (float)window_height, 0.1, 10000);
	free_camera->lookAt(Vector3(0, 25, 25), Vector3(0, 0, 0), Vector3(0, 1, 0));
	scene->loadLevel("data/scenes/space1.txt");
	player = scene->runner;
	player_camera->setPerspective(70, window_width / (float)window_height, 0.1, 10000);
	player_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 2, -5), player->getGlobalMatrix() * Vector3(0, 0, 20), Vector3(0, 1, 0));
	current_camera = player_camera;
	controller.clear();
	controller.push_back(new Controller(false));
	controller[0]->setTarget(this->player);
	controller[0]->setCamera(player_camera);
	player->controller = controller[0];
	for (int i = 0; i < scene->spitfire.size(); i++) {
		Controller* element = new Controller();
		element->setTarget(scene->spitfire[i]);
		//element->followTarget(player, Vector3(-30 + i * 15, 0, 0));
		controller.push_back(element);
		scene->spitfire[i]->controller = element;
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

	for (int i = 0; i < controller.size(); i++) {
		controller[i]->renderDebug();
	}

	scene->root->render(current_camera);
	shotManager->render(current_camera);

	if (currentStage == "defeat")
	{
		//Dibujamos texto en pantalla
		drawText(0.015*window_width, 0.025*window_height, "GAME OVER", Vector3(1, 0, 0), 16);
	}else{
		//Dibujamos texto en pantalla
		drawText(5, 5, "Outer Space", Vector3(1, 0, 0), 3);

		std::string impulse = "Impulse power: " + player->getImpulse() + "%";
		drawText(5, 25, impulse, Vector3(102 / 255, 255 / 255, 102 / 255), 2);

		std::string hull = "Hull damage: " + player->getHull() + "%";
		drawText(5, 45, hull, Vector3(102 / 255, 255 / 255, 102 / 255), 2);
	}


	//Funci�n para renderizar la interfaz
	renderGUI();

	/*glEnable(GL_BLEND);	
	if (debugPoints.size())
	{	
 		for (int i = 0; i < debugPoints.size(); i++)
		{
			
		}
	}
	glDisable(GL_BLEND);
	*/		

	glColor3f(1, 1, 1);
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//Renderizado de la interfaz
void Game::renderGUI()
{
	if (currentStage == "title")
	{
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		Camera cam2D;
		cam2D.setOrthographic(0, window_width, window_height, 0, -1, 1);
		cam2D.set();

		Mesh quad;
		quad.createQuad(window_width / 2, window_height / 2, window_width, window_height, true);

		Texture* tex = scene->textureManager->getTexture("data/hud/", "portada.tga");

		glEnable(GL_BLEND);
		tex->bind();
		quad.render(GL_TRIANGLES);
		tex->unbind();
		glDisable(GL_BLEND);
	}

	/*quad.createQuad(100, 100, 200, 200);
	Texture* tex = Texture::get("data / textures / island_color_luz.tga");
	tex->bind();
	quad.render(GL_TRIANGLES);
	tex->unbind();*/

	/*Mesh points;
	for (i = 0; fighters.size; i++)
	Fighter* fighter = fighters[i]
	Vector3 pos = fighter->getGlobalMatrix().getTranslation();
	pos = (pos / island->halfsize) * 200.0;
	pos.z = pos.y;
	pos.z = 0;
	pos.x += 100;
	pos.y += 100;
	points.vertices.push_back(pos);
	glColor(1, 0, 0, 1);
	points.render(GL_POINTS);*/
}

//Actualizamos los datos del juego
void Game::update(double seconds_elapsed)
{
	//Updates
	scene->root->update(seconds_elapsed);
	inputManager->update(seconds_elapsed);
	shotManager->update(seconds_elapsed*time);

	//Comprobamos colisiones
	collisionManager->check();

	//Borramos el contenedor con todo lo que se quiere destruir
	scene->clearRemovedEntities();	

	//Rotaci�n del planeta
	scene->planet->local_matrix.rotateLocal(seconds_elapsed / 50, Vector3(0, 1, 0));

	scene->spitfire.front()->current_velocity = 0;

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