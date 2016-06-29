#include "entity.h"

#include "includes.h"
#include "shader.h"
#include "scene.h"

std::vector<Entity*> Entity::toDestroy;
std::vector<Entity*> Entity::postOrderVector;


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

std::vector<Entity*> Entity::postOrder()
{
	//postOrderVector.clear();
	for (int i=0; i < this->children.size(); i++) {
		this->children[i]->postOrder();
	}
	postOrderVector.push_back(this);
	
	return postOrderVector;
}

float Entity::vehicleDistance(Vector3 position) {
	return -1;
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
		if (camera->testSphereInFrustum(pos, size) == true) //Hacemos frustum culling para pintar solo lo que hay en frustum
		{
			glPushMatrix(); //save the opengl state
			global_matrix.multGL(); //change the coordinates system
			if(texture != NULL)texture->bind(); //enable the texture	
			if (pos.distance(camera->eye) > 70 && mesh_low) //Escogemos la mesh de la calidad que toca
				mesh_low->render(GL_TRIANGLES);//render the mesh
			else
				mesh->render(GL_TRIANGLES);//render the mesh
			if (texture != NULL)texture->unbind(); //disable the texture
			glPopMatrix();
		}
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

float EntityMesh::vehicleDistance(Vector3 position) {
	return -1;
}

std::vector<EntityCollider*> EntityCollider::static_entities;
std::vector<EntityCollider*> EntityCollider::dynamic_entities;

EntityCollider::EntityCollider()
{
}

EntityCollider::~EntityCollider()
{
}

//Respuesta a eventos de colisiones
void EntityCollider::onShotCollision(float collisionPoint[3], float t1[9], float t2[9])
{
	std::cout << "ShotCollision: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
}

void EntityCollider::onEntityCollision(EntityCollider* entity, float collisionPoint[3], float t1[9], float t2[9])
{
	std::cout << "EntityCollision: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;

	Scene* scene = Scene::getInstance();

	//Asteroides
	/*auto it = find(scene->asteroides.begin(), scene->asteroides.end(), this);
	if (it != scene->asteroides.end())
	{
		Entity::destroyChild(this, 0.5);

		auto it2 = find(EntityCollider::static_entities.begin(), EntityCollider::static_entities.end(), this);
		EntityCollider::static_entities.erase(it2);

		scene->asteroides.erase(it);
	}*/
}

float EntityCollider::vehicleDistance(Vector3 position) {
	return -1;
}

