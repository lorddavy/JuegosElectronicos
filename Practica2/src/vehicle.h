#ifndef VEHICLE_H
#define VEHICLE_H

#include "includes.h";
#include "entity.h";

class Vehicle: public EntityMesh{
public:

	Vector3 velocity;
	int max_velocity;
	Vector3 impulse;
	int max_impulse;

	int hull;
	int shield;

	Vehicle();
	~Vehicle();

	void setup();	//	Le pasamos un fichero con todos los parametros y lo parseamos 

	void applyImpulse();

	void pitch();		// S and W keys
	void roll();		// A and D keys
	void yaw();			// Q and E keys

};

#endif