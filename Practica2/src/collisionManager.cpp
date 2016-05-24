#include "collisionManager.h"
#include <iostream>
#include <assert.h>

//Source code for the Collision Manager
CollisionManager* CollisionManager::instance = NULL;

CollisionManager::CollisionManager()
{
	assert(instance == NULL && "must be only one"); //must be only one, en release no petará
	std::cout << "CollisionManager created" << std::endl;
	instance = this;
	shotManager = ShotManager::getInstance();
}

//Comprobamos colisiones
void CollisionManager::check()
{
	/*for (int i = 0; i < EntityCollider::static_entities.size(); ++i)
	{
		for (int j = 1; j < shotManager->getMaxShots(); ++j)
		{
			Shot& shot = shotManager->shots[j];

			if (shot.active) {
				if (EntityCollider::static_entities[i]->mesh->testIntRayMesh(EntityCollider::static_entities[i]->getGlobalMatrix(), shot.origin_position, shot.end_position - shot.origin_position))
				{
					float collisionPoint[3];
					EntityCollider::static_entities[i]->mesh->collision_model->getCollisionPoint(collisionPoint, true);
					float t1[9], t2[9];
					EntityCollider::static_entities[i]->mesh->collision_model->getCollidingTriangles(t1, t2, false);
					std::cout << "Colisión en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
				}
			}
		}
	}*/
}

