#include "controller.h"

#include "game.h"
#include "entity.h"
#include "vehicle.h"
#include "camera.h"

Controller::Controller(bool ia) {
	target = NULL;
	camera = NULL;
	
	following = NULL;
	formation = Vector3(0, 0, 0);
	IA = ia;

	if (IA) {
		waypoints.clear();
		/*waypoints.push_back(Vector3(200, 0, 0));
		waypoints.push_back(Vector3(400, 0, 0));
		waypoints.push_back(Vector3(400, 0, 200));
		waypoints.push_back(Vector3(200, 0, 200));*/

		Vector3 v;
		for (int i = 0; i < 10; i++) {
			v.random(Vector3(500, 500, 500));
			waypoints.push_back(v);
		}
	}
	

}

Controller::~Controller() {

}

void Controller::update(double dt) {

	Game* game = Game::getInstance();
	const Uint8* keystate = game->keystate;
	
	if (!IA & game->current_camera == game->player_camera) {
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
			if (keystate[SDL_SCANCODE_SPACE]) {
				target->shoot('b');
				target->shoot('l');
			}


			//Camara jugador
			camera->lookAt(target->getGlobalMatrix() * Vector3(0, 15, -35),
				target->getGlobalMatrix() * Vector3(0, 0, 20),
				target->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)));
		}
	}
	else if (IA) {
		if (following != NULL) {
			updateFollowing(dt);
		}
		else {
			updateWaypoints(dt);
		}
		
	}
}

void Controller::setTarget(Vehicle* target) {
	this->target = target;
}

void Controller::setCamera(Camera* camera) {
	this->camera = camera;
}

void Controller::followTarget(Vehicle* follow, Vector3 delta) {
	following = follow;
	formation = delta;
	IA = true;
}

void Controller::updateFollowing(float dt) {
	target->pointerPosition(following->getGlobalMatrix() * formation, dt);
	Vector3 globalFollowingUp = following->getGlobalMatrix().rotateVector(Vector3(0, 1, 0));
	target->balanceVehicle(globalFollowingUp, dt);
}

void Controller::updateWaypoints(float dt) {
	Vector3 direction = waypoints.front() - target->getGlobalMatrix() * Vector3(0, 0, 0);
	float distance = direction.length();

	if (distance < 10) {
		Vector3 first = waypoints.front();
		waypoints.erase(waypoints.begin());
		waypoints.push_back(first);
	}
	this->target->pointerPosition(waypoints.front(), dt);
}

void Controller::renderDebug() {
	glLineWidth(1);
	glColor3f(1, 0.2, 0.2);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.2, 1, 0.2);

	if (waypoints.begin() != waypoints.end()) {
		//Print all waypoints
		if (1) {
			for (int i = 0; i < waypoints.size(); i++) {
				Vector3 currentVertex = waypoints[i];
				glVertex3f(currentVertex.x, currentVertex.y, currentVertex.z);
			}
		}

		//Print only next waypoint
		glColor3f(1, 0.2, 1);
		Vector3 currentVertex = waypoints.front();
		glVertex3f(currentVertex.x, currentVertex.y, currentVertex.z);
		currentVertex = target->getGlobalMatrix() * Vector3(0, 0, 0);
		glVertex3f(currentVertex.x, currentVertex.y, currentVertex.z);
		
	}
	glEnd();
	glColor3f(1, 1, 1);

}