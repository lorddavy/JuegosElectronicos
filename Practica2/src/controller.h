#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"
#include "framework.h"

class Vehicle;
class Camera;

class Controller
{
public:
	Controller(bool ia = true);
	~Controller();
	
	void update(double dt);

	void setTarget(Vehicle* target);
	void setCamera(Camera* camera);

	void followTarget(Vehicle* follow, Vector3 delta );
	void updateFollowing(float dt);

	void updateWaypoints(float dt);

	void renderDebug();

	void Controller::updateState(float dt);

private:
	Vehicle* target;
	Camera* camera;
	char* state;

	Vehicle* following;
	Vector3 formation;
	bool IA;
	std::vector<Vector3> waypoints;


};

#endif