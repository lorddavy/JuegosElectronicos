#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"

//class Vehilce;
class Camera;

class Controller
{
public:
	Controller();
	~Controller();
	
	void update(double dt);

private:
	//Vehicle* target;
	//Camera* camera;

};

#endif