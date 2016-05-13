#ifndef SHOTMANAGER_H
#define SHOTMANAGER_H

#include "framework.h""
#include "entity.h"
#include "shot.h"

#define MAX_SHOTS 50


class ShotManager{
public:
	Shot shots[MAX_SHOTS]; //std::vector<Bullet*>;

	static ShotManager* getInstance() {
		if (instance == NULL)
			instance = new ShotManager();
		return instance;
	};

	void createShot(char type, Vector3 origin, Vector3 end, Vector3 vel, float ttl, Entity* owner);

	void render(Camera* camera);
	void update(float dt);

private:
	static ShotManager* instance;
	ShotManager();

};

#endif