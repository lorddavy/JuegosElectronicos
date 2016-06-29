#include "entity.h"
#include "vehicle.h"
#include "shotManager.h"
#include "game.h"
#include "scene.h"
#include <algorithm>
#include <vector> 



Vehicle::Vehicle()
{
	velocity = Vector3(0, 0, 1);
	current_velocity = 30;
	max_velocity = 500;
	camera_eye.set(0, 2, -5);
	max_hull = 100;
	hull = max_hull;
	shield = 0;
	dead = false;
}

Vehicle::~Vehicle()
{
	
}

//Configuración inicial
/*void Vehicle::setup()
{
	
}*/

//Funciones de movimiento del vehiculo

void Vehicle::accelerate(float x)
{
	current_velocity += x;
	
	if (current_velocity > max_velocity){
		current_velocity = max_velocity;
	}
	else if (-current_velocity > max_velocity) {
		current_velocity = -max_velocity;
	}

	//std::cout << "Velocity: (" << this->velocity.x << ", " << this->velocity.y << ", " << this->velocity.z << ") " << std::endl;
	//std::cout << "current_velocity: " << this->current_velocity << std::endl << std::endl;
}

void Vehicle::pitch(float angle)
{
	this->local_matrix.rotateLocal(angle, Vector3(1, 0, 0));
}

void Vehicle::roll(float angle)
{
	this->local_matrix.rotateLocal(angle, Vector3(0, 0, 1));
}

void Vehicle::yaw(float angle)
{
	this->local_matrix.rotateLocal(angle, Vector3(0, 1, 0));
}

void Vehicle::stop()
{
	current_velocity = 0;
}

//Método de disparo del vehiculo
void Vehicle::shoot(char type)
{
	//if (!dead)
	//{
		ShotManager* shotManager = ShotManager::getInstance();
		//Disparo tipo beam
		if (type == 'b') {

			Matrix44 global = getGlobalMatrix();
			Vector3 origin = global * Vector3(0, 0, 10);

			Vector3 end = global * Vector3(0, 0, 500);
			Vector3 vel = global.rotateVector(Vector3(0, 0, 0));

			shotManager->createShot('b', origin, end, vel, 10, this);
		}
		//disparo tipo laser
		if (type == 'l')
		{
			Matrix44 global = getGlobalMatrix();
			Vector3 origin = global * Vector3(13, 0, -45);
			Vector3 end = global * Vector3(13, 0, 25);

			Vector3 vel = global.rotateVector(Vector3(0, 0, 50));

			shotManager->createShot('l', origin, end, vel, 45, this);

			origin = global * Vector3(-13, 0, -45);
			end = global * Vector3(-13, 0, 25);

			shotManager->createShot('l', origin, end, vel, 45, this);
		}
	//}
}

//Método render del vehiculo
void Vehicle::render(Camera* camera)
{
	//if (!dead)
	//{
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
				if (texture != NULL)texture->bind(); //enable the texture	
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
	//}
}

//Método update del vehiculo
void Vehicle::update(float dt)
{
	//if (!dead)
	//{

		velocity = this->getGlobalMatrix().rotateVector(Vector3(0, 0, -1));
		velocity.normalize();

		Vector3 translation = velocity * current_velocity * -dt;
		local_matrix.traslate(translation.x, translation.y, translation.z);

		if (hull < 0)
		{
			die();
		}
	//}
}

void Vehicle::die()
{
	Game* game = Game::getInstance();
	Scene* scene = Scene::getInstance();
	dead = true;

	if (game->player == this) game->end(); //Fin de juego

										   //Eliminamos la entidad y sus entidades hijas
	Entity::destroyChild(this, 0.5);

	//La quitamos del vector de Entity collider dinámico
	auto it = find(EntityCollider::dynamic_entities.begin(), EntityCollider::dynamic_entities.end(), this);
	EntityCollider::dynamic_entities.erase(it);

	//Eliminamos su controlador del vector de controladores
	auto it2 = find(game->controller.begin(), game->controller.end(), this->controller);
	game->controller.erase(it2);

	//La quitamos del vector de enemigos (si lo es)
	auto it3 = find(scene->enemies.begin(), scene->enemies.end(), this);
	if (it3 != scene->enemies.end())scene->enemies.erase(it3);
}

//Getters del vehiculo
std::string Vehicle::getVelocity() {	// No se usa
	std::stringstream ss;
	ss << (int)current_velocity / max_velocity * 100;
	return ss.str();
}

std::string Vehicle::getImpulse()
{
	int impulse;
	impulse = this->current_velocity/ this->max_velocity * 100;

	std::ostringstream strs;
	strs << impulse;
	return strs.str();
}

std::string Vehicle::getHull()
{
	int hull;
	hull = 100 - this->hull;

	std::ostringstream strs;
	strs << hull;
	return strs.str();
}

//Respuesta eventos de colisión
void Vehicle::onShotCollision(float collisionPoint[3], float t1[9], float t2[9])
{
	hull -= 20;
	std::cout << "Colisión en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
}
void Vehicle::onEntityCollision(EntityCollider* entity, float collisionPoint[3], float t1[9], float t2[9])
{
	hull -= 10;
	this->stop();
	std::cout << "Colisión en: " << collisionPoint[0] << ", " << collisionPoint[1] << ", " << collisionPoint[2] << std::endl;
}

//???

void Vehicle::pointerPosition(Vector3 target, float dt) {
	
	Vector3 position = this->getGlobalMatrix() * Vector3(0, 0, 0);
	Vector3 targetDirection = target - position;
	Matrix44 inverseMatrix = this->getGlobalMatrix();
	inverseMatrix.inverse();

	Vector3 globalAxis = velocity.cross(targetDirection);
	Vector3 localAxis = inverseMatrix.rotateVector(globalAxis);

	float angle = dt;
	
	this->local_matrix.rotateLocal(angle, localAxis);
	
}

void Vehicle::balanceVehicle(Vector3 targetUp, float dt) {
	targetUp.normalize();
	Vector3 wingDirection = this->getGlobalMatrix().rotateVector(Vector3(1, 0, 0));
	wingDirection.normalize();

	float correction = targetUp.dot(wingDirection);
	float angle = correction * dt * 0.5;

	this->roll(angle);

}

float Vehicle::vehicleDistance(Vector3 position) 
{
	Vector3 distanceVector = this->getGlobalMatrix() * Vector3(0, 0, 0) - position;
	
	return distanceVector.length();;
}

void Vehicle::heal(float dt)
{
	static float restore = 0;
	int factor = 5;

	if (hull < max_hull -1)
	{
		restore += factor * dt;
		if (restore >= 1) {
			restore--;
			hull ++;
			std::cout << "Hull: " << hull << std::endl;
		}
	}
	else {
		hull = max_hull;
	}
}