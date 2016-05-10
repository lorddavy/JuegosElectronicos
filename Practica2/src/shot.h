#ifndef SHOT_H
#define SHOT_H

#include "framework.h""
#include "entity.h"

#define MAX_SHOTS 50

class Shot{
public:

public:
	Vector3 position;
	Vector3 last_position;

	Vector3 velocity;
	char type;
	void* owner;
	float ttl; //tiempo de vida


	Shot(); //Constructor
	 ~Shot();	//Destructor
private:
};

#endif