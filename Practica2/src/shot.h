#ifndef SHOT_H
#define SHOT_H

#include "framework.h""
#include "entity.h"

#define MAX_SHOTS 50

class Shot{
public:

public:
	Vector3 origin_position;
	Vector3 end_position;	

	Vector3 velocity;
	char type;
	Entity* owner;
	float ttl; //tiempo de vida

	bool active;

	Shot(); //Constructor
	 ~Shot();	//Destructor
private:
};

#endif