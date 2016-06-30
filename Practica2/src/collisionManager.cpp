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
		for (int j = 0; j < shotManager->getMaxShots(); ++j)
		{
			Shot& shot = shotManager->shots[j];

			if (shot.active) {
				//Miramos si hay colisión con disparo activo
				if (EntityCollider::static_entities[i]->mesh->testIntRayMesh(EntityCollider::static_entities[i]->getGlobalMatrix(), shot.origin_position, shot.end_position - shot.origin_position, false, 0, 2))
				{
					//Tomamos punto y triangulos de colision
					float collisionPoint[3];
					EntityCollider::static_entities[i]->mesh->collision_model->getCollisionPoint(collisionPoint, true);
					float t1[9], t2[9];
					EntityCollider::static_entities[i]->mesh->collision_model->getCollidingTriangles(t1, t2, false);
					
					//Invocamos respuesta al evento
					//EntityCollider::static_entities[i]->onShotCollision(collisionPoint, t1, t2);
					shot.ttl = 0;
				}
			}
		}
	}

	//Colisiones para entidades dinamicas
	for (int i = 0; i < EntityCollider::dynamic_entities.size(); ++i)
	{
		//Tomamos los necesario para construir la esfera
		Matrix44 globalMatrix = EntityCollider::dynamic_entities[i]->getGlobalMatrix();
		Vector3 center = EntityCollider::dynamic_entities[i]->mesh->boundingBox.center;
		float radius = min(min(EntityCollider::dynamic_entities[i]->mesh->boundingBox.half_size.x, EntityCollider::dynamic_entities[i]->mesh->boundingBox.half_size.y), EntityCollider::dynamic_entities[i]->mesh->boundingBox.half_size.z);
		radius *= 1.2;
		//Colisiones entre entidades dinamicas y disparos
		for (int j = 0; j < shotManager->getMaxShots(); ++j)
		{
			Shot& shot = shotManager->shots[j];

			if (shot.active && shot.owner != EntityCollider::dynamic_entities[i]) {
				//Miramos si hay colisión con disparo activo
				if (EntityCollider::dynamic_entities[i]->mesh->testIntRayMesh(EntityCollider::dynamic_entities[i]->getGlobalMatrix(), shot.origin_position, shot.end_position - shot.origin_position, false, 0, 2))
				{
					//Tomamos punto y triangulos de colision
					float collisionPoint[3];
					EntityCollider::dynamic_entities[i]->mesh->collision_model->getCollisionPoint(collisionPoint, true);
					float t1[9], t2[9];
					EntityCollider::dynamic_entities[i]->mesh->collision_model->getCollidingTriangles(t1, t2, false);

					//Invocamos respuesta al evento
					EntityCollider::dynamic_entities[i]->onShotCollision(collisionPoint, t1, t2);
					if (shot.type == 'l')
						shot.ttl = 0;
					//else if (shot.type == 'b')
						//shot.end_position = shot.owner->getGlobalMatrix() * Vector3(collisionPoint[0], collisionPoint[1], collisionPoint[2]) - shot.owner->local_matrix.getTranslation();
				}
			}
		}
		
		//Colisiones entre entidades dinamicas y entidades estaticas
		for (int j = 0; j < EntityCollider::static_entities.size(); ++j)
		{
			if (EntityCollider::static_entities[j]->mesh->testIntSphereMesh(EntityCollider::static_entities[j]->getGlobalMatrix(), globalMatrix * center, radius))
			{
				//Tomamos punto y triangulos de colision
				float collisionPoint[3];
				EntityCollider::static_entities[j]->mesh->collision_model->getCollisionPoint(collisionPoint, true);
				float t1[9], t2[9];
				EntityCollider::static_entities[j]->mesh->collision_model->getCollidingTriangles(t1, t2, false);

				//Invocamos respuesta al evento				
				EntityCollider::dynamic_entities[i]->onEntityCollision(EntityCollider::static_entities[j], collisionPoint, t1, t2);
				//EntityCollider::static_entities[j]->onEntityCollision(EntityCollider::dynamic_entities[i], collisionPoint, t1, t2);
			}
		}

		//Colisiones entre entidades dinamicas y otras entidades dinamicas (SIN REPETICIONES)
		for (int j = i; j < EntityCollider::dynamic_entities.size(); ++j)
		{
			if (EntityCollider::dynamic_entities[j]!=EntityCollider::dynamic_entities[i])
			{
				if (EntityCollider::dynamic_entities[j]->mesh->testIntSphereMesh(EntityCollider::dynamic_entities[j]->getGlobalMatrix(), globalMatrix * center, radius))
				{
					//Tomamos punto y triangulos de colision
					float collisionPoint[3];
					EntityCollider::dynamic_entities[j]->mesh->collision_model->getCollisionPoint(collisionPoint, true);
					float t1[9], t2[9];
					EntityCollider::dynamic_entities[j]->mesh->collision_model->getCollidingTriangles(t1, t2, false);

					//Invocamos respuesta al evento
					EntityCollider::dynamic_entities[j]->onEntityCollision(EntityCollider::dynamic_entities[i], collisionPoint, t1, t2);
					EntityCollider::dynamic_entities[i]->onEntityCollision(EntityCollider::dynamic_entities[j], collisionPoint, t1, t2);
				}
			}
		}
	}
}

