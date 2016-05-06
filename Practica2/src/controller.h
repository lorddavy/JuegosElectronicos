#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"

class Game;

class Controller{
public:

	Controller();
	~Controller();

	void update(double dt);

};

#endif