#include <assert.h>

#include "inputManager.h"

#include "game.h"
#include "scene.h"
#include "controller.h"

InputManager* InputManager::instance = NULL;

InputManager::InputManager()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "InputManager created" << std::endl;
	instance = this;
}

void InputManager::update(double dt) {

	Game* game = Game::getInstance();
	Camera* current_camera = game->current_camera;
	std::vector <Controller*> controller = game->controller;
	const Uint8* keystate = game->keystate;

	double speed = dt * 100; //the speed is defined by the seconds_elapsed so it goes constant
	

	//Camara libre
	//mouse input to rotate the cam
	if ((game->mouse_state & SDL_BUTTON_LEFT) || game->mouse_locked) //is left button pressed?
	{
		game->free_camera->rotate(game->mouse_delta.x * 0.005, Vector3(0, -1, 0));
		game->free_camera->rotate(game->mouse_delta.y * 0.005, current_camera->getLocalVector(Vector3(-1, 0, 0)));
	}

	if (current_camera == game->free_camera) {
		//async input to move the camera around
		if (keystate[SDL_SCANCODE_LSHIFT]) speed *= 10; //move faster with left shift
		if (keystate[SDL_SCANCODE_W]) current_camera->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S]) current_camera->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A]) current_camera->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D]) current_camera->move(Vector3(-1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_R]) current_camera->move(Vector3(0, -1, 0) * speed);
		if (keystate[SDL_SCANCODE_F]) current_camera->move(Vector3(0, 1, 0) * speed);
	}
	else if(current_camera == game->player_camera)
		for (int i = 0; i < controller.size(); i++) {
			controller[i]->update(dt);
		}
	//to navigate with the mouse fixed in the middle
	if (game->mouse_locked)
	{
		int center_x = floor(game->window_width*0.5);
		int center_y = floor(game->window_height*0.5);
		//center_x = center_y = 50;
		SDL_WarpMouseInWindow(game->window, center_x, center_y); //put the mouse back in the middle of the screen
																	//SDL_WarpMouseGlobal(center_x, center_y); //put the mouse back in the middle of the screen

		game->mouse_position.x = center_x;
		game->mouse_position.y = center_y;
	}
	//}

}