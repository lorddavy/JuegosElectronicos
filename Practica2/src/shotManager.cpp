#include "shotManager.h"
#include <iostream>
#include <assert.h>

//Source code for the Shot Manager
ShotManager* ShotManager::instance = NULL;

ShotManager::ShotManager()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "MeshManager created" << std::endl;
	instance = this;
	memset(shots, 0, sizeof(Shot) * MAX_SHOTS);
}

void ShotManager::createShot(Vector3 pos, Vector3 vel, float ttl, void* owner)
{
	/*for (int i = 0; i < MAX_SHOTS; ++i)
	{
	Shot& shot = shots[i];
	if (shot.ttl > 0)
	continue;
	shot = pos;
	shot.last_pos = pos
	shot.vel = vel;
	shot.ttl = ttl;
	shot.owner = owner;

	}*/
}

void ShotManager::render(Camera* camera)
{
	/*Mesh mesh;
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
	Shot& shot = shots[i];
	if (shot.ttl >= 0)
	{
	mesh.vertices.push_back(shot.last_pos);
	mesh.vertices.push_back(shot.pos);
	mesh.colors.push_back(Vector4(0, 0, 0, 1))
	mesh.colors.push_back(Vector4(1, 1, 0.3, 1))

	}
	}
	if (mes.vertices.size() == 0)
	return;

	glLineWidth(3);
	glColor3f(1, 0, 1);
	mesh.render(GL_LINES);
	glColor3f(1, 1, 1);*/


}

void ShotManager::update(float dt)
{
	/*for (int i = 0; i < MAX_SHOTS; ++i)
	{
	Shot& shot = shots[i];
	//...
	}*/
}

