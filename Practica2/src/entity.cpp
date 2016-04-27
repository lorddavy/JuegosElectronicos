#include "entity.h"

#include "includes.h"

//Entity Class Methods

Entity::Entity()
{
	parent = NULL;
}

Entity::~Entity()
{
}

void Entity::render(Camera* camera)
{
	/*for (...children.size())
		children[i]->render(camera);*/
}

void Entity::update(float dt)
{
	/*for (...children.size())
		children[i]->update(dt);*/
}

void Entity::addEntity(Entity* e)
{
	e->parent = this;
	children.push_back(e);
}

//EntityMesh Class Methods

void EntityMesh::render(Camera* camera)
{
	if (mesh)
	{
		Matrix44 global_matrix = this->getGlobalMatrix();
		//Vector3 pos = global_matrix.getTranslation(); //Por hacer función
		//Vector3 center = global_matrix * mesh->boundingBox.center();

		//….. (código de render)

	}
}


