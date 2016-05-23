#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "framework.h""
#include "entity.h"
#include "shot.h"

class CollisionManager{
public:
	std::vector<EntityMesh*> static_entities;
	std::vector<EntityMesh*> dynamic_entities;

	static CollisionManager* getInstance() {
		if (instance == NULL)
			instance = new CollisionManager();
		return instance;
	};

	void update(float dt);

private:
	static CollisionManager* instance;
	CollisionManager();

};

#endif