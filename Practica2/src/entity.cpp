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
		Vector3 pos = global_matrix.getTranslation(); //Por hacer función
		//Vector3 center = global_matrix * mesh->boundingBox.center();

													  //….. (código de render)
	}
}

void EntityMesh::update(float dt)
{

}

void EntityMesh::setup(const char* mesh, const char* texture, const char* mesh_low = NULL)
{
	this->mesh = MeshManager::getInstance()->getMesh(mesh);
	if (texture)
		this->texture = TextureManager::getInstance()->getTexture(texture);
	if (mesh_low)
		this->mesh_low = MeshManager::getInstance()->getMesh(mesh_low);

	this->mesh->uploadToVRAM();
	this->mesh_low->uploadToVRAM();
}



