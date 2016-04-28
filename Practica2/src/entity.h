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

	virtual void render(Camera* camera);
	virtual void update(float dt);

	void addEntity(Entity*);
	void removeEntity(Entity*);
	//Vector3 getPosition();

	Matrix44 getGlobalMatrix()
	{
		if (parent)
			global_matrix = parent->local_matrix * getGlobalMatrix();
		else
			global_matrix = local_matrix;
		return global_matrix;
	}
};

class EntityMesh : public Entity {
public:
	Mesh* mesh;
	Mesh* low_mesh;
	Texture* texture;
	Vector3* color;	

	bool frustum_text = true;

	EntityMesh();
	~EntityMesh();

	void render(Camera* camera);
	void update(float dt);

	void setup(const char* mesh, const char* texture, const char* low_mesh = NULL)
	{
		//this->mesh = this->mesh->get(mesh);
		/*if (texture)
		this->texture = ...;
		if (low_mesh)
		this->low_mesh = ...;*/
	}
};


#endif