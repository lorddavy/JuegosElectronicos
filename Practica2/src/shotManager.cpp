#include "shotManager.h"
#include "vehicle.h"
#include <iostream>
#include <assert.h>

//Source code for the Shot Manager
ShotManager* ShotManager::instance = NULL;

ShotManager::ShotManager()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "ShotManager created" << std::endl;
	instance = this;
	memset(shots, 0, sizeof(Shot) * MAX_SHOTS);
}

void ShotManager::createShot(char type, Vector3 origin, Vector3 end, Vector3 vel, float ttl, Entity* owner)
{
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		Shot& shot = shots[i];
		
		if (type == 'b')
		{
			Shot& shot = shots[0];
			shot.type = 'b';
			shot.origin_position = origin;
			shot.end_position = end;
			shot.velocity = vel;
			shot.ttl = ttl;
			shot.owner = owner;
			shot.active = true;
		}
	}
}

void ShotManager::render(Camera* camera)
{
	Mesh mesh;
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
	Shot& shot = shots[i];

		if (shot.active && shot.ttl > 0)
		{
			mesh.vertices.push_back(shots[0].origin_position);			
			mesh.vertices.push_back(shots[0].end_position);	
			mesh.colors.push_back(Vector4(1, 0.3, 0.5, 1));	
			mesh.colors.push_back(Vector4(0, 0, 1, 1));		

			glLineWidth(4);
			glColor3f(1, 0, 1);
			mesh.render(GL_LINES);
			glColor3f(1, 1, 1);
		}
	}
	if (mesh.vertices.size() == 0)
		return;	


}

void ShotManager::update(float dt)
{
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		Shot& shot = shots[i];

		if (shot.active)
		{
			if (shot.ttl > 0)
			{
				//std::cout << shot.ttl << "\n" << std::endl;
				if (shot.type == 'b')
				{
					shot.ttl -= 1;
					shot.origin_position = shot.owner->getGlobalMatrix()*Vector3(0, 0, 10);
					shot.end_position = shot.owner->getGlobalMatrix()*Vector3(0, 0, 500);
				}
			}
			else {
				shot.active = false;
			}
		}
	}
}

int ShotManager::getMaxShots()
{
	return MAX_SHOTS;
}