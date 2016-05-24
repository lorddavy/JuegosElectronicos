#include "entity.h"

#include "includes.h"
#include "shader.h"

std::vector<Entity*> Entity::toDestroy;

//Entity Class Methods
Entity::Entity()
{
	parent = NULL;
}

Entity::~Entity()
{
	for (int i = 0; i < children.size(); i++)
		delete children[i];

	this->parent->removeChild(this);
}

void Entity::render(Camera* camera)
{
	for (int i = 0; i < children.size(); i++)
		children[i]->render(camera); //repeat for every child
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

void Entity::destroyChild(Entity* ent, float time)
{
	this->removeChild(ent);
	toDestroy.push_back(ent);
}

void Entity::removeChild(Entity* ent) {
	for (int i = 0; i < this->children.size(); i++) {
		if (this->children[i] == ent) {
			this->children.erase(this->children.begin() + i);
		}
	}
}

Matrix44 Entity::getGlobalMatrix()
{
	if (parent)
		return local_matrix * parent->getGlobalMatrix();
	return local_matrix;
}

//EntityMesh Class Methods

EntityMesh::EntityMesh()
{
	parent = NULL;
	texture = NULL;
	mesh_low = NULL;
}

EntityMesh::~EntityMesh()
{
}

void EntityMesh::render(Camera* camera)
{
	if (mesh)
	{
		global_matrix = getGlobalMatrix();
		Vector3 pos = local_matrix.getTranslation(); 
		Vector3 center = global_matrix * mesh->boundingBox.center;		
		
		float size = max(max(mesh->boundingBox.half_size.x, mesh->boundingBox.half_size.y), mesh->boundingBox.half_size.z);
		//if (camera->testSphereInFrustum(pos, size) == true) //Hacemos frustum culling para pintar solo lo que hay en frustum
		//{
			glPushMatrix(); //save the opengl state
			global_matrix.multGL(); //change the coordinates system
			if(texture != NULL)texture->bind(); //enable the texture	
			if (pos.distance(camera->eye) > 100 && mesh_low) //Escogemos la mesh de la calidad que toca
				mesh_low->render(GL_TRIANGLES);//render the mesh
			else
				mesh->render(GL_TRIANGLES);//render the mesh
			if (texture != NULL)texture->unbind(); //disable the texture
			glPopMatrix();
		//}
	}		

	for (int i = 0; i < children.size(); i++)
		children[i]->render(camera); //repeat for every child
}

void EntityMesh::update(float dt)
{

}

void EntityMesh::setup(const char* path, const char* mesh, const char* texture, const char* mesh_low)
{
	this->mesh = MeshManager::getInstance()->getMesh(path, mesh);
	this->mesh->uploadToVRAM();

	if (texture)
		this->texture = TextureManager::getInstance()->getTexture(path, texture);
	else
		this->texture = NULL;
	if (mesh_low)
	{
		this->mesh_low = MeshManager::getInstance()->getMesh(path, mesh_low);
		this->mesh_low->uploadToVRAM();
	}
	else {
		this->mesh_low = NULL;
	}
}

EntityCollider::EntityCollider()
{
}

EntityCollider::~EntityCollider()
{
}

void EntityCollider::onEntityCollision()
{
}

void EntityCollider::onShotCollision()
{
}

