#include "controller.h"
#include "game.h"

Controller::Controller() {

}

void Controller::update(double dt) {

	Game* game = Game::getInstance();
	
	const Uint8* keystate = game->keystate;
	Camera* camera = game->camera;






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

		/*Matrix44 global_player_matrix = player->getGlobalMatrix();
		Vector3 detras = global_player_matrix* Vector3(0, 2, -5);
		detras = camera->eye * 0.1 + detras *0.9;
		camera->lookAt(detras, global_player_matrix* Vector3(0, 0, 20), global_player_matrix.rotateVector(Vector3(0, 1, 0)));*/

		camera->lookAt(game->player->getGlobalMatrix() * Vector3(0, 2, -5), game->player->getGlobalMatrix() * Vector3(0, 0, 20), Vector3(0, 1, 0));

		//Control del jugador
		/*if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) camera->move(Vector3(0, 0, 1) * speed);
		if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) camera->move(Vector3(0, 0, -1) * speed);
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