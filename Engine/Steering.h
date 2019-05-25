#pragma once

#include "Vec3.h"

#include <vector>

class EnemyParent;
class NodeAStar;
class Steering {
public:
	// returns a velocity to seek to destination position, seeking velocity = desired velocity - current velocity
	static Vecf3 Seek(EnemyParent* entity);
	Vecf3 Seek(EnemyParent* entity, const Vecf3& endPos);
	// returns a container of grid cells that fall within feeler range, currentVelocity is assumed to also be the heading
	static std::vector<Veci2> FeelerGridCollision(EnemyParent* enemy);
	// returns if feelers collide with grid cell marked solid
	static bool FeelerCollideSolid(EnemyParent* entity, const std::vector<Veci2>& feelerCells, NodeAStar*& node);
	// calculate obstacle avoid steering force
	Vecf3 AvoidObstacles(const float& incomingMagnitude, const Vecf3& centerPosition, const Vecf3& collidePosition);
	// calculate seperation force
	Vecf3 Flocking(EnemyParent* entity);
	// process feeler logic
	static void ProcessFeelers(EnemyParent* entity);
	// accumulated steering force
	static Vecf3 CalculateSteering(EnemyParent* entity);
private:
	
};