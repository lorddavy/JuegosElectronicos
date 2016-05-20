#include "controller.h"

#include "game.h"
#include "entity.h"
#include "vehicle.h"
#include "camera.h"

Controller::Controller() {
	target = NULL;
	camera = NULL;
}

Controller::~Controller() {

}

void Controller::update(double dt) {

	Game* game = Game::getInstance();
	const Uint8* keystate = game->keystate;

	double speed = dt * 100; //the speed is defined by the seconds_elapsed so it goes constant

	int pitchInverted = -1;

	if (target != NULL) {
		//Acelerar
		if (keystate[SDL_SCANCODE_R]) target->accelerate(0.5 * speed);
		if (keystate[SDL_SCANCODE_F]) target->accelerate(-0.5 * speed);
		//Pitch
		if (keystate[SDL_SCANCODE_W]) target->pitch(pitchInverted * 0.01 * speed);
		if (keystate[SDL_SCANCODE_S]) target->pitch(pitchInverted * -0.01 * speed);
		//Roll
		if (keystate[SDL_SCANCODE_A]) target->roll(0.01 * speed);
		if (keystate[SDL_SCANCODE_D]) target->roll(-0.01 * speed);
		//Yaw
		if (keystate[SDL_SCANCODE_Q]) target->yaw(-0.01 * speed);
		if (keystate[SDL_SCANCODE_E]) target->yaw(0.01 * speed);
		//Stop
		if (keystate[SDL_SCANCODE_X]) target->stop();
		//Shooting Beam
		if (keystate[SDL_SCANCODE_SPACE]) target->shoot('b');

		//Camara jugador
		if (camera != NULL) {
			camera->lookAt(target->getGlobalMatrix() * Vector3(0, 15, -35),
				target->getGlobalMatrix() * Vector3(0, 0, 20),
				target->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)));
		}
	}
}

void Controller::setTarget(Vehicle* target) {
	this->target = target;
}

void Controller::setCamera(Camera* camera) {
	this->camera = camera;
}