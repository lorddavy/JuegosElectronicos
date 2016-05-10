#include "inputManager.h"
#include "game.h"

#include "vehicle.h"

InputManager::InputManager() {

}

void InputManager::update(double dt) {

	Game* game = Game::getInstance();
	const Uint8* keystate = game->keystate;
	Camera* camera = game->camera;
	Vehicle* player = (Vehicle*) game->player;

	double speed = dt * 100; //the speed is defined by the seconds_elapsed so it goes constant
	
	//Camara libre
	if (game->cameraType == 0)
	{
		//mouse input to rotate the cam
		if ((game->mouse_state & SDL_BUTTON_LEFT) || game->mouse_locked) //is left button pressed?
		{
			camera->rotate(game->mouse_delta.x * 0.005, Vector3(0, -1, 0));
			camera->rotate(game->mouse_delta.y * 0.005, camera->getLocalVector(Vector3(-1, 0, 0)));
		}

		//async input to move the camera around
		if (keystate[SDL_SCANCODE_LSHIFT]) speed *= 10; //move faster with left shift
		if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) camera->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) camera->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) camera->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) camera->move(Vector3(-1, 0, 0) * speed);

	}

	//Camara jugador
	if (game->cameraType == 1)
	{
		camera->lookAt(game->player->getGlobalMatrix() * Vector3(0, 2, -5), game->player->getGlobalMatrix() * Vector3(0, 0, 20), Vector3(0, 1, 0));

		//Control del jugador
		int pitchInverted = -1;
		if (keystate[SDL_SCANCODE_W]) player->pitch(pitchInverted * 0.01 * speed);
		if (keystate[SDL_SCANCODE_S]) player->pitch(pitchInverted * -0.01 * speed);
		if (keystate[SDL_SCANCODE_A]) player->roll(0.01 * speed);
		if (keystate[SDL_SCANCODE_D]) player->roll(-0.01 * speed);
		if (keystate[SDL_SCANCODE_Q]) player->yaw(-0.01 * speed);
		if (keystate[SDL_SCANCODE_E]) player->yaw(0.01 * speed);
		if (keystate[SDL_SCANCODE_R]) player->accelerate(0.5 * speed);
		if (keystate[SDL_SCANCODE_F]) player->accelerate(-0.5 * speed);

		/*if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) camera->move(Vector3(0, 0, -1) * speed);
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) camera->move(Vector3(1, 0, 0) * speed);
		if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) camera->move(Vector3(-1, 0, 0) * speed);*/

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
	}

	
}