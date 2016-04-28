#include "entity.h"

#include "includes.h"
#include "shader.h"

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

Matrix44 Entity::getGlobalMatrix()
{
	/*if (this->parent)
		return  local_matrix * parent->getGlobalMatrix();*/
	return this->global_matrix;
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
		Mesh* render_mesh = this->mesh;	
		float angle = 0;
		
		Matrix44 global_matrix = this->getGlobalMatrix();
		Vector3 pos = global_matrix.getTranslation(); //Por hacer función
		//Vector3 center = global_matrix * mesh->boundingBox.center();
		//Matrix44 m;
		//m.setScale(1, 1, 1);
		//m.rotate(angle * DEG2RAD, Vector3(0, 1, 0)); //build a rotation matrix
		//Matrix44 mvp = m * camera->viewprojection_matrix;

		if (pos.distance(camera->eye) > 100) //Escogemos la mesh de la calidad que toca
		{
			render_mesh = this->mesh_low;
		}
				
		float size = max(max(render_mesh->boundingBox.half_size.x, render_mesh->boundingBox.half_size.y), render_mesh->boundingBox.half_size.z);
		if (camera->testSphereInFrustum(pos, size) == true) //Renderizamos solo si está en frustum
		{
			glPushMatrix(); //save the opengl state
			global_matrix.multGL(); //change the coordinates system
			this->texture->bind(); //enable the texture			
			render_mesh->render(GL_TRIANGLES);//render the mesh
			this->texture->unbind(); //disable the texture
			glPopMatrix();
		}
	}		

	for (int i = 0; i < children.size(); i++)
		children[i]->render(camera); //repeat for every child
}

void EntityMesh::update(float dt)
{

}

void EntityMesh::setup(const char* mesh, const char* texture, const char* mesh_low)
{
	this->mesh = MeshManager::getInstance()->getMesh(mesh);
	if (texture)
		this->texture = TextureManager::getInstance()->getTexture(texture);
	if (mesh_low)
		this->mesh_low = MeshManager::getInstance()->getMesh(mesh_low);

	this->mesh->uploadToVRAM();
	this->mesh_low->uploadToVRAM();
}



