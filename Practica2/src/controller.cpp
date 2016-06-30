#include "controller.h"

#include "game.h"
#include "scene.h"
#include "entity.h"
#include "vehicle.h"
#include "camera.h"


Controller::Controller(bool ia) {
	target = NULL;
	camera = NULL;
	state = NULL;
	
	following = NULL;
	//formation = Vector3(0, 0, 0);
	IA = ia;

	if (IA) {
		state = "patrol";

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
	target = NULL;
	camera = NULL;
	state = NULL;
	following = NULL;
	delete state;
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
			//if (keystate[SDL_SCANCODE_A]) target->roll(0.01 * speed);
			//if (keystate[SDL_SCANCODE_D]) target->roll(-0.01 * speed);
			if (keystate[SDL_SCANCODE_A]) target->roll(0.005 * speed);
			if (keystate[SDL_SCANCODE_D]) target->roll(-0.005 * speed);
			//Yaw
			//if (keystate[SDL_SCANCODE_Q]) target->yaw(-0.01 * speed);
			//if (keystate[SDL_SCANCODE_E]) target->yaw(0.01 * speed);
			if (keystate[SDL_SCANCODE_Q]) target->yaw(-0.005 * speed);
			if (keystate[SDL_SCANCODE_E]) target->yaw(0.005 * speed);
			//Stop
			if (keystate[SDL_SCANCODE_X]) target->stop();
			//Shooting Beam
			if (keystate[SDL_SCANCODE_SPACE]) {
				//target->shoot('b');
				target->shoot('l');
			}


			//Camara jugador
			camera->lookAt(target->getGlobalMatrix() * Vector3(0, 15, -35),
				target->getGlobalMatrix() * Vector3(0, 0, 20),
				target->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)));
		}
	}
	else if (IA)
	{
		if (clock(dt)) {
			evaluateState();
		}
		updateState(dt);
		
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

void Controller::updateFollowing(float dt)
{
	if (following != NULL) {
		target->pointerPosition(following->getGlobalMatrix() * formation, dt);
		Vector3 globalFollowingUp = following->getGlobalMatrix().rotateVector(Vector3(0, 1, 0));
		target->balanceVehicle(globalFollowingUp, dt);
	}
}

void Controller::updateRunAway(float dt)
{
	Vector3 myPos = target->getGlobalMatrix() * Vector3(0, 0, 0);
	Vector3 followingPos = following->getGlobalMatrix() * Vector3(0, 0, 0);
	Vector3 runAway /*= myPos - direction (followingPos - myPos)*/ = myPos * 2 - followingPos;

	target->pointerPosition(runAway, dt);
	Vector3 globalFollowingUp = following->getGlobalMatrix().rotateVector(Vector3(0, 1, 0));
	target->balanceVehicle(globalFollowingUp, dt);
}

void Controller::updateWaypoints(float dt)
{
	target->current_velocity = 30;

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

void Controller::updateState(float dt)
{
	if (state == "patrol") {
		updateWaypoints(dt);
	}
	else if(state == "attack"){
		updateFollowing(dt);
	}
	else if (state == "shooting") {
		updateFollowing(dt);
		shotEnemy(dt);
	}
	else if (state == "scape") {
		updateRunAway(dt);
	}
	else if (state == "heal") {
		target->heal(dt);
	}
	else {
		std::cout << "Controller ERROR: Unknown state" << std::endl;
	}
}

void Controller::evaluateState()
{
	std::cout << std::endl << "State: " << state << std::endl;
	Vehicle* enemyClose = enemyAtDistance(10000);
	std::cout << "Hull: " << target->hull << std::endl <<std::endl;

	if (state == "patrol") {
		// Enemigo cerca?
		Vehicle* enemy = enemyAtDistance(1000);
		if (enemy != NULL) {
			if (Game::getInstance()->player == enemy) {
				following = enemy;
				state = "attack";
				std::cout << "State: " << state << std::endl;
			}
		}
	}
	else if(state == "attack" || state == "shooting"){
		Vehicle* enemy = enemyAtDistance(500);
		if (target->hull < target->max_hull * 0.5) {
			state = "scape";
		}
		else if (state == "attack" && enemy != NULL) {
			if (following != enemy) {
				following = enemy;
			}
			state = "shooting";
		}
		else if (state == "shooting") {
			state = "attack";
		}

		std::cout << "State: " << state << std::endl;
	}
	else if (state == "scape") {
		Vehicle* enemy = enemyAtDistance(1100);
		if (enemy == NULL) {
			state = "heal";
		}
		std::cout << "State: " << state << std::endl;

	}
	else if (state == "heal"){
		Vehicle* enemy = enemyAtDistance(1000);
		if (target->hull == target->max_hull) {
			state = "patrol";
		}
		else if (enemy != NULL)
		{
			if (target->hull < target->max_hull * 0.8) {
				state = "scape";
			}
			else{
				state = "attack";
			}
		}
		std::cout << "State: " << state << std::endl;
	}
}

Vehicle* Controller::enemyAtDistance(float dist)
{
	Entity* root = Scene::getInstance()->root;
	// Recorremos el árbol en postorden (izquierda, derecha, padre) -> el último elemento es root
	root->postOrderVector.clear();
	std::vector<Entity*> postOrderVector = root->postOrder();

	Vehicle* closest = NULL;
	float minDist = dist;

	for (int i = 0; i < postOrderVector.size(); i++) {
		Vector3 pos = postOrderVector[i]->getGlobalMatrix() * Vector3(0, 0, 0);
		/*std::cout << "Entity position: (" <<
			pos.x << ", " <<
			pos.y << ", " <<
			pos.z << ") " << std::endl;*/
		Vector3 myPos = target->getGlobalMatrix() * Vector3(0, 0, 0);
		float vehicleDistance = postOrderVector[i]->vehicleDistance(myPos);

		if (vehicleDistance > 1) {
			std::cout << "Distance: " << vehicleDistance << std::endl;
		}

		if (vehicleDistance > 1 && vehicleDistance < dist) {
			closest = (Vehicle*) postOrderVector[i];
			minDist = vehicleDistance;
		}

	}

	return closest;
}

bool Controller::clock(float dt) {
	static float last_time = 0;
	static int seconds = 0;
	last_time += dt;
	if (last_time >= 1) {
		seconds++;
		last_time -= 1;
		//std::cout << seconds << std::endl;
		return true;
	}
	return false;
}

void Controller::shotEnemy(float dt) {
	target->shoot('l');
}