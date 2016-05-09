#include "vehicle.h"

// Implementamos los métodos de vehicle.h

Vehicle::Vehicle()
{
	velocity = Vector3(0,0,1);
	current_velocity = 30;
	max_velocity = 100;
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

	velocity = getGlobalMatrix() * Vector3(0, 0, -1);
	velocity.normalize();
	
	if (current_velocity > max_velocity){
		current_velocity = max_velocity;
	}

	std::cout << "Velocity: (" << this->velocity.x << ", " << this->velocity.y << ", " << this->velocity.z << ") " << std::endl;
	std::cout << "current_velocity: " << this->current_velocity << std::endl << std::endl;
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

void Vehicle::update(float dt)
{
	Vector3 translation = velocity * current_velocity * -dt;
	local_matrix.traslate(translation.x, translation.y, translation.z);
}


