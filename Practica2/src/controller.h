#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"

class Entity;
class Camera;

class Controller
{
public:
	Controller();
	~Controller();

	Entity* target;
	Camera* camera;

};

#endif