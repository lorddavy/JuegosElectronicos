#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"

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

//private:
	Vehicle* target;
	Camera* camera;

};

#endif