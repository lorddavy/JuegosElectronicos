#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "includes.h"

class Game;

class InputManager{
public:

	InputManager();
	~InputManager();

	void update(double dt);

};

#endif