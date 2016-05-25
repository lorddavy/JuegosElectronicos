#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"
#include "framework.h"

class Vehicle;
class Camera;

class Controller
{
public:
	Controller();
	~Controller();
	
	void update(double dt);

	void setTarget(Vehicle* target);
	void setCamera(Camera* camera);

	void followTarget(Vehicle* follow, Vector3 delta );
	void updateFollowing(float dt);

//private:
	Vehicle* target;
	Camera* camera;

	Vehicle* following;
	Vector3 formation;
	bool AI;

};

#endif