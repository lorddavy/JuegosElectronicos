#ifndef SHOTMANAGER_H
#define SHOTMANAGER_H

#include "framework.h""
#include "entity.h"
#include "shot.h"

#define MAX_SHOTS 300


class ShotManager{
public:
	Shot shots[MAX_SHOTS];

	static ShotManager* getInstance() {
		if (instance == NULL)
			instance = new ShotManager();
		return instance;
	};

	void createShot(char type, Vector3 origin, Vector3 end, Vector3 vel, float ttl, Entity* owner);

	void render(Camera* camera);
	void update(float dt);

	int getMaxShots();

private:
	static ShotManager* instance;
	ShotManager();

};

#endif