#include "entity.h"
#include "vehicle.h"
#include "shotManager.h"
#include "game.h"
#include <algorithm>
#include <vector> 



Vehicle::Vehicle()
{
	velocity = Vector3(0, 0, 1);
	current_velocity = 30;
	max_velocity = 500;
	camera_eye.set(0, 2, -5);
	hull = 100;
	shield = 0;
}

Vehicle::~Vehicle()
{
	
}

//Configuración inicial
/*void Vehicle::setup()
{
	
}*/

//Funciones de movimiento del vehiculo

void Vehicle::accelerate(float x)
{
	current_velocity += x;
	
	if (current_velocity > max_velocity){
		current_velocity = max_velocity;
	}
	else if (-current_velocity > max_velocity) {
		current_velocity = -max_velocity;
	}

	//std::cout << "Velocity: (" << this->velocity.x << ", " << this->velocity.y << ", " << this->velocity.z << ") " << std::endl;
	//std::cout << "current_velocity: " << this->current_velocity << std::endl << std::endl;
}

void Vehicle::pitch(float angle)
{
	this->local_matrix.rotateLocal(angle, Vector3(1, 0, 0));
}

void Vehicle::roll(float angle)
{
	this->local_matrix.rotateLocal(angle, Vector3(0, 0, 1));
}

void Vehicle::yaw(float angle)
{
	this->local_matrix.rotateLocal(angle, Vector3(0, 1, 0));
}

void Vehicle::stop()
{
	current_velocity = 0;
}

//Método de disparo del vehiculo
void Vehicle::shoot(char type)
{
	if (hull > 0)
	{
		ShotManager* shotManager = ShotManager::getInstance();
		//Disparo tipo beam
		if (type == 'b') {

			Matrix44 global = getGlobalMatrix();
			Vector3 origin = global * Vector3(0, 0, 10);

			Vector3 end = global * Vector3(0, 0, 500);
			Vector3 vel = global.rotateVector(Vector3(0, 0, 0));

			shotManager->createShot('b', origin, end, vel, 10, this);
		}
		//disparo tipo laser
		if (type == 'l')
		{
			Matrix44 global = getGlobalMatrix();
			Vector3 origin = global * Vector3(13, 0, -12);
			Vector3 end = global * Vector3(13, 0, 20);

			Vector3 vel = global.rotateVector(Vector3(0, 0, 15));

			shotManager->createShot('l', origin, end, vel, 45, this);

			origin = global * Vector3(-13, 0, -12);
			end = global * Vector3(-13, 0, 20);

			shotManager->createShot('l', origin, end, vel, 45, this);
		}
	}
}
//Método update del vehiculo
void Vehicle::update(float dt)
{
	velocity = this->getGlobalMatrix().rotateVector(Vector3(0, 0, -1));
	velocity.normalize();

	Vector3 translation = velocity * current_velocity * -dt;
	local_matrix.traslate(translation.x, translation.y, translation.z);

	if (hull < 0)
	{
		Game* game = Game::getInstance();
		
		if (game->player == this) game->end(); //Fin de juego

		Entity::destroyChild(this, 0.5);

		auto it = find(EntityCollider::dynamic_entities.begin(), EntityCollider::dynamic_entities.end(), this);
		EntityCollider::dynamic_entities.erase(it);

		auto it2 = find(game->controller.begin(), game->controller.end(), this->controller);
		game->controller.erase(it2);
	}
}

//Getters del vehiculo
std::string Vehicle::getVelocity() {	// No se usa
	std::stringstream ss;
	ss << (int)current_velocity / max_velocity * 100;
	return ss.str();
}

std::string Vehicle::getImpulse()
{
	int impulse;
	impulse = this->current_velocity/ this->max_velocity * 100;

	std::ostringstream strs;
	strs << impulse;
	return strs.str();
}

std::string Vehicle::getHull()
{
	int hull;
	hull = 100 - this->hull;

	std::ostringstream strs;
	strs << hull;
	return strs.str();
}

//Respuesta eventos de colisión
void Vehicle::onShotCollision(float collisionPoint[3], float t1[9], float t2[9])
{
	hull -= 20;
	std::cout << "Colisión en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
}
void Vehicle::onEntityCollision(EntityCollider* entity, float collisionPoint[3], float t1[9], float t2[9])
{
	hull -= 10;
	this->stop();
	std::cout << "Colisión en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
}

//???

void Vehicle::pointerPosition(Vector3 target, float dt) {
	
	Vector3 position = this->getGlobalMatrix() * Vector3(0, 0, 0);
	Vector3 targetDirection = target - position;
	Matrix44 inverseMatrix = this->getGlobalMatrix();
	inverseMatrix.inverse();

	Vector3 globalAxis = velocity.cross(targetDirection);
	Vector3 localAxis = inverseMatrix.rotateVector(globalAxis);

	float angle = dt;
	
	this->local_matrix.rotateLocal(angle, localAxis);
	
}

void Vehicle::balanceVehicle(Vector3 targetUp, float dt) {
	targetUp.normalize();
	Vector3 wingDirection = this->getGlobalMatrix().rotateVector(Vector3(1, 0, 0));
	wingDirection.normalize();

	float correction = targetUp.dot(wingDirection);
	float angle = correction * dt * 0.5;

	this->roll(angle);

}