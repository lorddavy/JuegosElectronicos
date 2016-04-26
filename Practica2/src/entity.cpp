#include "entity.h"

#include "includes.h"

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

void EntityMesh::render(Camera* camera)
{
	if (mesh)
	{
		Matrix44 global_matrix = this->getGlobalMatrix();
		Vector3 pos = global_matrix.getTranslation();
		Vector3 center = global_matrix * mesh->center();

		//….. (código de render)

	}
}
