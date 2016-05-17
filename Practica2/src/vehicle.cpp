#include "vehicle.h"
#include "shotManager.h"

Vehicle::Vehicle()
{
	velocity = Vector3(0,0,1);
	current_velocity = 30;
	max_velocity = 500;
	camera_eye.set(0, 2, -5);
}

Vehicle::~Vehicle()
{
}

/*void Vehicle::setup()
{
}*/

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

void Vehicle::update(float dt)
{
	velocity = this->getGlobalMatrix().rotateVector(Vector3(0, 0, -1));
	velocity.normalize();

	Vector3 translation = velocity * current_velocity * -dt;
	local_matrix.traslate(translation.x, translation.y, translation.z);
}

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

void Vehicle::shoot(char type)
{
	if (type == 'b')
	{
		ShotManager* shotManager = ShotManager::getInstance();
		Matrix44 global = getGlobalMatrix();
		Vector3 origin = global * Vector3(0, 0, 10);

		Vector3 end = global * Vector3(0, 0, 500);
		Vector3 vel = global.rotateVector(Vector3(0, 0, 0));

		shotManager->createShot('b', origin, end, vel, 10, this);
	}
}

