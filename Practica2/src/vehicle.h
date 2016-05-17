#ifndef VEHICLE_H
#define VEHICLE_H

#include "includes.h"
#include "entity.h"

class Vehicle: public EntityMesh{
public:

	Vector3 velocity;
	float max_velocity;
	float current_velocity;

	Vector3 impulse;
	int max_impulse;

	Vector3 camera_eye;

	int hull;
	int shield;

	Vehicle(); //Constructor
	 ~Vehicle();	//Destructor

	//void setup();	//	Le pasamos un fichero con todos los parametros y lo parseamos 

	void accelerate(float x);

	void pitch(float angle);		// S and W keys
	void roll(float angle);			// A and D keys
	void yaw(float angle);			// Q and E keys
	void stop();					//X key

	void shoot(char type);			// SPACE key

	void update(float dt);

	std::string getVelocity();
	std::string getImpulse();

private:

};

#endif