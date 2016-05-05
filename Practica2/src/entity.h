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

	unsigned int uid;
	std::string name;
	Matrix44 local_matrix;
	Matrix44 global_matrix;
	std::vector<Entity*> children;
	Entity* parent;

	virtual void render(Camera* camera);
	virtual void update(float dt);

	void addEntity(Entity*);
	void removeChild(Entity* ent); //No tiene que destruir nada, solo lo quita del árbol, lo desvincula, pero luego se puede poner a otro sitio
	void destroyChild(Entity* ent, float time); // time: cuanto tiempo quiero que espere hasta destruir la entidad

	//Vector3 getPosition();

	Matrix44 getGlobalMatrix();
	
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

	void setup(const char* mesh, const char* texture, const char* mesh_low = NULL);
};


#endif