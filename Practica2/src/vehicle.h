#ifndef VEHICLE_H
#define VEHICLE_H

#include "includes.h"
#include "entity.h"
#include "controller.h"

class Vehicle: public EntityCollider{
public:

	Vector3 velocity;
	float max_velocity;
	float current_velocity;

	Vector3 impulse;
	int max_impulse;

	Vector3 camera_eye;

	int hull;
	int shield;
	//bool dead;

	Vehicle(); //Constructor
	 ~Vehicle();	//Destructor

	//void setup();	//	Le pasamos un fichero con todos los parametros y lo parseamos 

	//Movimientos
	Controller* controller;

	void accelerate(float x);
	void pitch(float angle);		// S and W keys
	void roll(float angle);			// A and D keys
	void yaw(float angle);			// Q and E keys
	void stop();					//X key
	void shoot(char type);			// SPACE key

	void pointerPosition(Vector3 target, float dt);
	void balanceVehicle(Vector3 targetUp, float dt);

	//render & update
	void render(Camera* camera);
	void update(float dt);
	void die();

	//Getters
	std::string getVelocity();
	std::string getImpulse();
	std::string getHull();

	//Respuesta a eventos de colision
	void onShotCollision(float collisionPoint[3], float t1[9], float t2[9]);
	void onEntityCollision(EntityCollider* entity, float collisionPoint[3], float t1[9], float t2[9]);

private:

};

#endif