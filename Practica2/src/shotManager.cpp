#include "shotManager.h"
#include "vehicle.h"
#include <iostream>
#include <assert.h>

//Source code for the Shot Manager
ShotManager* ShotManager::instance = NULL;

ShotManager::ShotManager()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petar�
	std::cout << "ShotManager created" << std::endl;
	instance = this;
	memset(shots, 0, sizeof(Shot) * MAX_SHOTS);
}

void ShotManager::createShot(char type, Vector3 origin, Vector3 end, Vector3 vel, float ttl, Entity* owner)
{
	//Disparo de tipo beam
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

		return;
	}

	//Disparos de tipo laser
	for (int i = 1; i < MAX_SHOTS; ++i)
	{
		Shot& shot = shots[i];		
		
		if (type == 'l')
		{
			if (shot.active == true)
				continue;

			shot.type = 'l';
			shot.origin_position = origin;
			shot.end_position = end;
			shot.velocity = vel;
			shot.ttl = ttl;
			shot.owner = owner;
			shot.active = true;

			return;
		}
	}
}

void ShotManager::render(Camera* camera)
{	
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
	Mesh mesh;
	Shot& shot = shots[i];

		if (shot.active && shot.ttl > 0)
		{
			//Render de disparo beam
			if (shot.type == 'b')
			{
				Mesh mesh;
				mesh.vertices.push_back(shots[0].origin_position);
				mesh.vertices.push_back(shots[0].end_position);
				mesh.colors.push_back(Vector4(102 / 255, 255 / 255, 102 / 255, 1));
				mesh.colors.push_back(Vector4(95 / 255, 250 / 255, 95 / 255, 1));

				if (mesh.vertices.size() == 0)
					return;

				glLineWidth(4);
				glColor3f(1, 0, 1);
				mesh.render(GL_LINES);
				glColor3f(1, 1, 1);
			}
			//Render de disparos laser
			if (shot.type == 'l')
			{
				mesh.vertices.push_back(shots[i].origin_position);
				mesh.vertices.push_back(shots[i].end_position);
				mesh.colors.push_back(Vector4(1, 1, 0.3, 1));
				mesh.colors.push_back(Vector4(1, 0, 0, 1));				

				if (mesh.vertices.size() == 0)
					return;

				glLineWidth(2);
				glColor3f(1, 0, 1);
				mesh.render(GL_LINES);
				glColor3f(1, 1, 1);

			}
		}
	}
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
				//update de disparo beam
				if (shot.type == 'b')
				{
					shot.ttl -= 1;
					shot.origin_position = shot.owner->getGlobalMatrix() * Vector3(0, 0, 10);
					shot.end_position = shot.owner->getGlobalMatrix() * Vector3(0, 0, 500);
				}
				//update de disparos laser
				if (shot.type == 'l')
				{
					shot.ttl -= 1;
					shot.origin_position = shot.origin_position + shot.velocity;
					shot.end_position = shot.end_position + shot.velocity;
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