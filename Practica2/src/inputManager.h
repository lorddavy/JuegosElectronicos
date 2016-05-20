#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "includes.h"

class Game;
class Controller;

class InputManager{
public:
	static InputManager* getInstance() {
		if (instance == NULL)
			instance = new InputManager();
		return instance;
	};

	void update(double dt);

private:
	static InputManager* instance;
	InputManager();
	//Controller* controller;
};

#endif