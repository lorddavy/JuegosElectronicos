#include "collisionManager.h"
#include "entity.h"
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
	//Colisiones entre entidades estaticas y disparos
	for (int i = 0; i < EntityCollider::static_entities.size(); ++i)
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
	}

	//Colisiones entre entidades dinamicas y disparos
	for (int i = 0; i < EntityCollider::dynamic_entities.size(); ++i)
	{
		for (int j = 1; j < shotManager->getMaxShots(); ++j)
		{
			Shot& shot = shotManager->shots[j];

			if (shot.active) {
				if (EntityCollider::dynamic_entities[i]->mesh->testIntRayMesh(EntityCollider::dynamic_entities[i]->getGlobalMatrix(), shot.origin_position, shot.end_position - shot.origin_position))
				{
					float collisionPoint[3];
					EntityCollider::dynamic_entities[i]->mesh->collision_model->getCollisionPoint(collisionPoint, true);
					float t1[9], t2[9];
					EntityCollider::dynamic_entities[i]->mesh->collision_model->getCollidingTriangles(t1, t2, false);
					std::cout << "Colisión en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
				}
			}
		}
	}

	//Colisiones entre entidades estaticas y entidades dinamicas
	for (int i = 0; i < EntityCollider::dynamic_entities.size(); ++i)
	{
		Matrix44 globalMatrix = EntityCollider::dynamic_entities[i]->getGlobalMatrix();
		Vector3 center = EntityCollider::dynamic_entities[i]->mesh->boundingBox.center;
		float radius = EntityCollider::dynamic_entities[i]->mesh->boundingBox.half_size.z;

		for (int j = 0; j < EntityCollider::static_entities.size(); ++j)
		{
				if (EntityCollider::static_entities[j]->mesh->testIntSphereMesh(EntityCollider::static_entities[j]->getGlobalMatrix(), globalMatrix * center, radius))
				{
						float collisionPoint[3];
						EntityCollider::static_entities[j]->mesh->collision_model->getCollisionPoint(collisionPoint, true);
						float t1[9], t2[9];
						EntityCollider::static_entities[j]->mesh->collision_model->getCollidingTriangles(t1, t2, false);
						std::cout << "Colisión en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;

				}
				
		}
	}	
}

