#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include "utils.h"
//#include "includes.h"
#include "mesh.h"
#include "meshManager.h"
#include "textureManager.h"
#include "texture.h"
#include "camera.h"


class Mesh;
class Texture;
class Camera;


class Entity
{
public:

	Entity(); //Constructor
	~Entity();	//Destructor

	static std::vector<Entity*> toDestroy;
	static std::vector<Entity*> postOrderVector;

	unsigned int uid;
	std::string name;
	Matrix44 local_matrix;
	Matrix44 global_matrix;
	std::vector<Entity*> children;
	Entity* parent;

	virtual void render(Camera* camera);
	virtual void update(float dt);
	

	void addEntity(Entity*);
	void removeChild(Entity* ent); //No tiene que destruir nada, solo lo quita del �rbol, lo desvincula, pero luego se puede poner a otro sitio
	void destroyChild(Entity* ent, float time); // time: cuanto tiempo quiero que espere hasta destruir la entidad

	//Vector3 getPosition();

	Matrix44 getGlobalMatrix();

	std::vector<Entity*> postOrder();
	virtual float vehicleDistance(Vector3 position);

};

class EntityMesh : public Entity {
public:
	Mesh* mesh;
	Mesh* mesh_low;
	Texture* texture;
	Vector3* color;

	bool frustum_text = true;
	bool two_sided = true;

	EntityMesh();
	~EntityMesh();

	void render(Camera* camera);
	void update(float dt);

	void setup(const char* path, const char* mesh, const char* texture = NULL, const char* mesh_low = NULL);

	virtual float vehicleDistance(Vector3 position);
};

class EntityCollider : public EntityMesh {
public:
	
	static std::vector<EntityCollider*> static_entities;
	static std::vector<EntityCollider*> dynamic_entities;

	EntityCollider();
	~EntityCollider();

	//Respuesta a eventos de colision
	virtual void onShotCollision(float collisionPoint[3], float t1[9], float t2[9]);
	virtual void onEntityCollision(EntityCollider* entity, float collisionPoint[3], float t1[9], float t2[9]);

	virtual float vehicleDistance(Vector3 position);
};

#endif