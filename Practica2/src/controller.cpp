#include "controller.h"

#include "game.h"
#include "entity.h"
#include "vehicle.h"
#include "camera.h"

Controller::Controller() {
	//target = NULL;
	//camera = NULL;
}

Controller::~Controller() {

}

void Controller::update(double dt) {

	Game* game = Game::getInstance();
	//Camera* current_camera = game->current_camera;
	Vehicle* player = game->player;
	Controller* controller = game->controller;

	const Uint8* keystate = game->keystate;

	double speed = dt * 100; //the speed is defined by the seconds_elapsed so it goes constant

	int pitchInverted = -1;

	if (player != NULL) {
		//Acelerar
		if (keystate[SDL_SCANCODE_R]) player->accelerate(0.5 * speed);
		if (keystate[SDL_SCANCODE_F]) player->accelerate(-0.5 * speed);
		//Pitch
		if (keystate[SDL_SCANCODE_W]) player->pitch(pitchInverted * 0.01 * speed);
		if (keystate[SDL_SCANCODE_S]) player->pitch(pitchInverted * -0.01 * speed);
		//Roll
		if (keystate[SDL_SCANCODE_A]) player->roll(0.01 * speed);
		if (keystate[SDL_SCANCODE_D]) player->roll(-0.01 * speed);
		//Yaw
		if (keystate[SDL_SCANCODE_Q]) player->yaw(-0.01 * speed);
		if (keystate[SDL_SCANCODE_E]) player->yaw(0.01 * speed);
		//Stop
		if (keystate[SDL_SCANCODE_X]) player->stop();
		//Shooting Beam
		if (keystate[SDL_SCANCODE_SPACE]) player->shoot('b');

		//Camara jugador
		game->player_camera->lookAt(player->getGlobalMatrix() * Vector3(0, 15, -35),
			player->getGlobalMatrix() * Vector3(0, 0, 20),
			player->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)));
	}
}