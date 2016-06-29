
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

#include "bass.h"

//free_camera, player_camera, current_camera !!

//some globals
Game* Game::instance = NULL;
Scene* scene = NULL;

//Managers
InputManager* inputManager = NULL;
ShotManager* shotManager = NULL;
CollisionManager* collisionManager = NULL;

//El handler para el sample y canal de musica
HSAMPLE titleMusicSample;
HCHANNEL titleMusicChannel;
HSAMPLE battleMusicSample;
HCHANNEL battleMusicChannel;

//DEBUG
EntityCollider* debugEntityMesh;
EntityMesh* testMesh;
float loadTime;

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

	//2D camera
	cam2D = new Camera();
	cam2D->setOrthographic(0, window_width, window_height, 0, -1, 1);

	//Player_camera
	player_camera = new Camera();
	player_camera->setPerspective(70, window_width / (float)window_height, 0.1, 10000);

	scene = Scene::getInstance();

	//Managers
	scene->meshManager = MeshManager::getInstance();
	scene->textureManager = TextureManager::getInstance();
	inputManager = InputManager::getInstance();
	shotManager = ShotManager::getInstance();
	collisionManager = CollisionManager::getInstance();

	//Etapa inicial
	currentStage = "title";		
	current_camera = player_camera;

	//Opción de menú inicial
	menuOption = 0;
	missionText = 0;

	//Música y Sonido

	//Inicializamos BASS  (id_del_device, muestras por segundo, ...)
	const char* titleFilename = "data/music/intro.wav";	
	const char* battleFilename = "data/music/battle_music.wav";

	if (BASS_Init(-1, 44100, BASS_DEVICE_DEFAULT, 0, NULL))
	{
		BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, 1000.0);
		//BASS_SetVolume(1);

		//Cargamos samples (memoria, filename, offset, length, max, flags)
		titleMusicSample = BASS_SampleLoad(false, titleFilename, 0, 0, 3, BASS_SAMPLE_LOOP);
		if (titleMusicSample == 0)
		{
			int err = BASS_ErrorGetCode();
			std::cout << "ERROR AL CARGAR SONIDO: " << err << std::endl;
			return;
		}

		battleMusicSample = BASS_SampleLoad(false, battleFilename, 0, 0, 3, BASS_SAMPLE_LOOP);
		if (battleMusicSample == 0)
		{
			int err = BASS_ErrorGetCode();
			std::cout << "ERROR AL CARGAR SONIDO: " << err << std::endl;
			return;
		}

		//Creamos canales para samples
		titleMusicChannel = BASS_SampleGetChannel(titleMusicSample, false);
		battleMusicChannel = BASS_SampleGetChannel(battleMusicSample, false);

		//Lanzamos sample de musica del menú
		BASS_ChannelPlay(titleMusicChannel, false);
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
	
	if (scene->enemies.size() != 0)
	{
		currentStage = "defeat";
	}
	else {
		currentStage = "victory";
	}
	//scene = Scene::getInstance();
	//delete scene;	
}

//Reinicio del juego
bool Game::load(void)
{
	//Eliminamos la escena anterior
	//scene->root->removeChild(scene->root);

	//Seteamos a los valores iniciales de la escena
	
	scene->loadLevel("data/scenes/space1.txt");
	player = scene->runner;
	player_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 2, -5), player->getGlobalMatrix() * Vector3(0, 0, 20), Vector3(0, 1, 0));
	current_camera = player_camera;
	//Player controller	

	for (auto it = controller.begin(); it != controller.end(); ++it)
	{
		delete *it;
	}
	controller.clear();

	controller.push_back(new Controller(false));
	controller[0]->setTarget(this->player);
	controller[0]->setCamera(player_camera);
	player->controller = controller[0];//Puntero a su controlador
	//player->dead = false;

	//Enemies Controllers
	for (int i = 0; i < scene->enemies.size(); i++) {		
		Controller* element = new Controller();
		element->setTarget(scene->enemies[i]);
		//element->followTarget(player, Vector3(-30 + i * 15, 0, 0));
		controller.push_back(element);
		scene->enemies[i]->controller = element;//Puntero a su controlador

		//scene->enemies[i]->dead = false;
	}

	//Paramos Musica Menu 
	BASS_ChannelPause(titleMusicChannel);
	BASS_ChannelPlay(battleMusicChannel, true);
	BASS_ChannelPause(battleMusicChannel);

	std::cout << "Game Loaded" << std::endl;
	loadTime = time;

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
	for (int i = 0; i < scene->enemies.size(); i++) {
	Controller* element = new Controller();
	element->setTarget(scene->enemies[i]);
	//element->followTarget(player, Vector3(-30 + i * 15, 0, 0));
	controller.push_back(element);
	scene->enemies[i]->controller = element;
	}*/
	return true;
}

//what to do when the image has to be draw
void Game::render(void)
{
	if (currentStage == "game" || currentStage == "defeat" || currentStage == "victory")
	{
		if (time > loadTime + 12)
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

			//drawGrid(500); //background grid

			for (int i = 0; i < controller.size(); i++) {
				controller[i]->renderDebug();
			}

			scene->root->render(current_camera);
			shotManager->render(current_camera);
		}
	}

	//Función para renderizar la interfaz
	renderGUI();

	glColor3f(1, 1, 1);
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//Renderizado de la interfaz
void Game::renderGUI()
{
	//El HUD es distinto dependiendo de la Stage actual
	if (currentStage == "game")
	{
		//Dibujamos texto en pantalla
		drawText(5, 5, "Outer Space", Vector3(1, 0, 0), 3);

		std::string impulse = "Impulse power: " + player->getImpulse() + "%";
		drawText(5, 25, impulse, Vector3(102 / 255, 255 / 255, 102 / 255), 2);

		std::string hull = "Hull damage: " + player->getHull() + "%";
		drawText(5, 45, hull, Vector3(102 / 255, 255 / 255, 102 / 255), 2);

		std::string enemiesAlive = "Enemies remaining: " + scene->getEnemiesAlive();
		drawText(5, 65, enemiesAlive, Vector3(102 / 255, 255 / 255, 102 / 255), 2);
	}
	else if (currentStage == "defeat")
	{
		//Dibujamos texto en pantalla
		drawText(window_width / 70, window_height / 40, "GAME OVER", Vector3(1, 0, 0), 16);
		drawText(5, 5, "Press ESC key to return to the menu!", Vector3(1, 1, 1), 2);
	}
	else if (currentStage == "victory")
	{
		//Dibujamos texto en pantalla
		drawText(window_width/55, window_height/60, "VICTORY", Vector3(1, 1, 0), 16);
		drawText(window_width/50, window_height/20, "Thanks for playing!", Vector3(0, 1, 1), 10);
		drawText(5, 5, "Press ESC key to return to the menu!", Vector3(1, 1, 1), 2);
	}
	else if (currentStage == "title" || currentStage == "load" || currentStage == "menu" || currentStage == "credits" || currentStage == "mission")
	{
		char* texFile = "";
		//rutas para asset de textura
		if (currentStage == "title")
			texFile = "portada.tga";
		else if (currentStage == "load")
			texFile = "loading.tga";		
		else if (currentStage == "menu")
			texFile = "menu.tga";
		else if (currentStage == "credits")
			texFile = "credits.tga";
		else if (currentStage == "mission")
			texFile = "mission.tga";

		//render del quad 2D de HUD
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		cam2D->set();

		Mesh hud;
		hud.createQuad(window_width / 2, window_height / 2, window_width, window_height, true);

		Texture* tex = scene->textureManager->getTexture("data/hud/", texFile);

		glEnable(GL_BLEND);
		tex->bind();
		hud.render(GL_TRIANGLES);
		tex->unbind();
		glDisable(GL_BLEND);

		if (currentStage == "load") {
			drawText(window_width / 55, window_height / 40, "LOADING", Vector3(1, 1, 0), 16);
		}
		//Opciones del menú
		else if(currentStage == "menu"){ 
			if(menuOption == 0)
				texFile = "menuOption0.tga";
			if (menuOption == 1)
				texFile = "menuOption1.tga";
			if (menuOption == 2)
				texFile = "menuOption2.tga";
			/*if (menuOption == 3)
				texFile = "menuOption3.tga";*/
			if (menuOption == 3)
				texFile = "menuOption4.tga";

			tex = scene->textureManager->getTexture("data/hud/", texFile);

			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); //para toda pantalla
			tex->bind();
			hud.render(GL_TRIANGLES);
			tex->unbind();
			glDisable(GL_BLEND);

			//Dibujamos texto en pantalla
			drawText(5, 5, "Press ARROW keys to change the options and ENTER key to select one!", Vector3(1, 1, 1), 2);
		}
		else if (currentStage == "title" ) {
			drawText(5, 5, "Press any key!", Vector3(1, 1, 1), 2);
		}
		else if (currentStage == "mission") {
			
			
			drawText(5, 5, "Press any key!", Vector3(1, 1, 1), 2);

			//Text of the mission
			if (missionText >= 0)
				drawText(25, 60, "Mission -1Ex6x9B2- Report:", Vector3(0.36, 0.75, 0.56), 3);
			if (missionText > 0)
				drawText(25, 75, "Congratulations on your promotion , commander.", Vector3(0.36, 0.75, 0.56), 3); 
			if (missionText > 1)
				drawText(25, 95, "We hope you meet our expectations on you.", Vector3(0.36, 0.75, 0.56), 3);
			if (missionText > 2)
			{
				drawText(25, 115, "You should know the strategic importance that the Omicron-Delta sector has ", Vector3(0.36, 0.75, 0.56), 3);
				drawText(25, 125, "for the Human Cosmic Empire.", Vector3(0.36, 0.75, 0.56), 3);
			}
			if (missionText > 3)
			drawText(25, 135, "Recent discoveries of large ore veins are of great value to us.", Vector3(0.36, 0.75, 0.56), 3);
			if (missionText > 4)
			{
				drawText(25, 145, "A few days ago, we sent a fleet of mineral collectors but have been attacked by surprise", Vector3(0.36, 0.75, 0.56), 3);
				drawText(25, 155, "by what appear to be space pirates.", Vector3(0.36, 0.75, 0.56), 3);
			}
			if (missionText > 5)
				drawText(25, 165, "Your mission is to end them and safeguard our ships while are returning to our mothership.", Vector3(0.36, 0.75, 0.56), 3);
			if (missionText > 6)
				drawText(25, 175, "You are alone. Your proven experience should be more than enough.", Vector3(0.36, 0.75, 0.56), 3);
			if (missionText > 7)
			{
				drawText(25, 195, "Good luck,", Vector3(0.36, 0.75, 0.56), 3);
				drawText(25, 215, "HCE - Order and Progress", Vector3(0.36, 0.75, 0.56), 3);
			}
		}
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
	if (currentStage == "game" || currentStage == "defeat" || currentStage == "victory")
	{
		if (time > loadTime+12)
		{
			if (currentStage == "game")
			{
				//Peproducimos batalla
				BASS_ChannelPlay(battleMusicChannel, false);
			}

			//Updates
			scene->root->update(seconds_elapsed);
			inputManager->update(seconds_elapsed);
			shotManager->update(seconds_elapsed*time);

			//Comprobamos colisiones
			collisionManager->check();

			//Rotación del planeta
			if (scene->planet != NULL)
			{
				scene->planet->local_matrix.rotateLocal(seconds_elapsed / 100, Vector3(0, 1, 0));
			}

			if (scene->enemies.size() == 0)
			{
				end();
			}
			else
			{
				//scene->enemies.front()->current_velocity = 0;
			}
		}
	}
	else if (currentStage == "load")
	{
		renderGUI();
		//Cargamos el juego
		if (load())
		{
			currentStage = "game";
		}
		else
		{
			std::cout << "Error Loading Game" << std::endl;
		}
	}
	else if (currentStage == "title")
	{
		if (BASS_ChannelIsActive(titleMusicChannel) != BASS_ACTIVE_PLAYING)
		{
			//Musica
			BASS_ChannelPause(battleMusicChannel);
			BASS_ChannelPlay(titleMusicChannel, true);
		}
	}

	//Borramos el contenedor con todo lo que se quiere destruir
	scene->clearRemovedEntities();
}

//Keyboard event handler (sync input)
void Game::onKeyPressed(SDL_KeyboardEvent event)
{

	if (currentStage == "game")
	{
		switch (event.keysym.sym)
		{
			/*case SDLK_ESCAPE:
			if(currentStage == "game") currentStage = "menu";		//ESC key, opens the menu
			if(currentStage == "menu") currentStage = "game";*/
		case SDLK_TAB:												//Change free camera-player camera
			if (current_camera == free_camera)
				current_camera = player_camera;
			else if (current_camera == player_camera) {
				free_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 15, -35),
					player->getGlobalMatrix() * Vector3(0, 0, 20),
					player->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)));
				current_camera = free_camera;
			}
			break;
		}
	}
	else if (currentStage == "menu")
	{
		switch (event.keysym.sym)
		{
			//Opción anterior
		case SDLK_LEFT:
		case SDLK_UP:
			--menuOption;
			if (menuOption < 0) menuOption = 4;
			break;
			//Opción siguiente
		case SDLK_RIGHT:
		case SDLK_DOWN:
			++menuOption;
			if (menuOption > 3) menuOption = 0;
			break;
		case SDLK_RETURN:
			if (menuOption == 0)
			{
				currentStage = "mission";;
			}
			if (menuOption == 1)
			{
				currentStage = "load";			//empezar juego
			}
			else if (menuOption == 2)
			{

			}
			else if (menuOption == 3)
			{
				currentStage = "credits";		//Salir
			}
			/*else if (menuOption == 4)
			{
				
			}*/
			break;
		case SDLK_ESCAPE:
			currentStage = "title";					//ESC, ir a pantalla de titulo	
			break;
			//default: ;
		}
	}
	else if (currentStage == "defeat" || currentStage == "victory")
	{
		switch (event.keysym.sym)
		{
		case SDLK_ESCAPE:
			currentStage = "menu";										//ESC, ir a pantalla de titulo
			break;
		}
	}
	else if (currentStage == "title")
	{
		switch (event.keysym.sym)
		{
		case SDLK_ESCAPE: 
			currentStage = "credits";									//ESC key, Salir
			break;
		default: 
			currentStage = "menu";										//Otra tecla, ir al menu
			break;
		}
	}	
	else if (currentStage == "credits")
	{
		switch (event.keysym.sym)
		{									
		default:
			exit(0);						//cualquier tecla, Salir
			break;
		}
	}
	else if (currentStage == "mission")
	{
		switch (event.keysym.sym)
		{
		default:
			++missionText;
			if(missionText>8)
				currentStage = "menu";							//cualquier tecla, al menú
			break;
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