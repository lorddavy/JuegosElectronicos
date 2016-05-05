#include "vehicle.h"

// Implementamos los métodos de vehicle.h

Vehicle::Vehicle()
{
	velocity = Vector3(0,0,5);
	camera_eye.set(0, 2, -5);
}

Vehicle::~Vehicle()
{
}

/*void Vehicle::setup()
{
}*/

void Vehicle::applyImpulse()
{
}

void Vehicle::pitch()
{
}

void Vehicle::roll()
{
}

void Vehicle::yaw()
{
}

void Vehicle::update(float dt)
{
	local_matrix.traslate(-velocity.x*dt, -velocity.y*dt, -velocity.z*dt);
}


