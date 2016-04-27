#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include "utils.h"
//#include "includes.h"
#include "mesh.h"
#include "texture.h"
#include "camera.h"


class Mesh;
class Texture;
class Camera;


class Entity
{
public:

	Entity(); //Constructor
	virtual ~Entity();	//Destructor

	unsigned int uid;
	std::string name;
	Matrix44 local_matrix;
	Matrix44 global_matrix;
	std::vector<Entity*> children;
	Entity* parent;

	void addEntity(Entity*);
	Vector3 getPosition();

	virtual void render(Camera* camera);
	virtual void update(float dt);

	Matrix44 getGlobalMatrix()
	{
		if (parent)
			global_matrix = parent->local_matrix * getGlobalMatrix();
		else
			global_matrix = local_matrix;
		return global_matrix;
	}

	void removeEntity(Entity* e)
	{
		/*e->parent = this;
		children.push_back(e);*/
	}

};

class EntityMesh : public Entity {
public:
	Mesh* mesh;
	Mesh* low_mesh;
	Texture* texture;
	Vector3* color;

	void render(Camera* camera);
	void update(float dt);

	EntityMesh();
	virtual ~EntityMesh();
};


#endif