#pragma once

#include "Vec3.h"

#include <vector>

class EnemyAAOne;
class NodeAStar;
class Steering {
public:
	// returns a velocity to seek to destination position, seeking velocity = desired velocity - current velocity
	static Vecf3 Seek(EnemyAAOne* entity);

	// returns a container of grid cells that fall within feeler range, currentVelocity is assumed to also be the heading
	static std::vector<Veci2> FeelerGridCollision(EnemyAAOne* enemy);
	// returns if feelers collide with grid cell marked solid
	static bool FeelerCollideSolid(EnemyAAOne* entity, const std::vector<Veci2>& feelerCells, NodeAStar*& node);
	// calculate obstacle avoid steering force
	Vecf3 AvoidObstacles(const float& incomingMagnitude, const Vecf3& centerPosition, const Vecf3& collidePosition);
	// process feeler logic
	static void ProcessFeelers(EnemyAAOne* entity);
	// accumulated steering force
	static Vecf3 CalculateSteering(EnemyAAOne* entity);
private:
	
};