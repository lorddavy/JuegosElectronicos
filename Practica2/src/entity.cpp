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
	for (int i = 0; i < children.size(); i++)
		children[i]->render(camera);
}

void Entity::update(float dt)
{
	for (int i = 0; i < children.size(); i++)
		children[i]->update(dt);
}

void Entity::addEntity(Entity* e)
{
	e->parent = this;
	children.push_back(e);
}

//EntityMesh Class Methods

EntityMesh::EntityMesh()
{
	parent = NULL;
}

EntityMesh::~EntityMesh()
{

}

void EntityMesh::render(Camera* camera)
{
	if (mesh)
	{
		Matrix44 global_matrix = this->getGlobalMatrix();
		Vector3 pos = global_matrix.getTranslation(); //Por hacer funci�n
		//Vector3 center = global_matrix * mesh->boundingBox.center();

													  //�.. (c�digo de render)
	}
}

void EntityMesh::update(float dt)
{

}



