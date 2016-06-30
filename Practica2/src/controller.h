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
	void updateRunAway(float dt);
	void shotEnemy(float dt);

	void updateWaypoints(float dt);

	void renderDebug();

	void updateState(float dt);
	void evaluateState();
	Vehicle* enemyAtDistance(float dist);
	float computeDistanceToEnemy(Vehicle* enemy);

	Vehicle* following;

	bool clock(float dt);

private:
	Vehicle* target;
	Camera* camera;
	char* state;

	
	Vector3 formation;
	bool IA;
	std::vector<Vector3> waypoints;


};

#endif