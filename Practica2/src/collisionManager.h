#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "framework.h""
#include "shotManager.h"


class CollisionManager{
public:
	static CollisionManager* getInstance() {
		if (instance == NULL)
			instance = new CollisionManager();
		return instance;
	};

	ShotManager* shotManager = NULL;

	void check();

private:
	static CollisionManager* instance;
	CollisionManager();

};

#endif