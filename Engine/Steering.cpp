#include "Steering.h"

#include "EnemyAAOne.h"
#include "TerrainWithPath.h"

Vecf3 Steering::Seek(EnemyParent* entity)
{
	// calculate desired velocity
	Vecf3 desiredVelocity = (entity->autonomousAgentDestination - entity->GetSpawnLocationOffset()).GetNormalized() * entity->speed;
	// get and return steering velocity
	Vecf3 steeringForce = (desiredVelocity - entity->currentVelocity).GetNormalized();
	// setting vertical force to 0
	steeringForce.y = 0.0f;
	// limit the steering force
	return steeringForce * entity->turningForce;
}

Vecf3 Steering::Seek(EnemyParent* entity, const Vecf3& endPos)
{
	// calculate desired velocity
	Vecf3 desiredVelocity = (endPos - entity->GetSpawnLocationOffset()).GetNormalized() * entity->speed;
	// get and return steering velocity
	Vecf3 steeringForce = (desiredVelocity - entity->currentVelocity).GetNormalized();
	// setting vertical force to 0
	steeringForce.y = 0.0f;
	// limit the steering force
	return steeringForce * entity->turningForce;
}

std::vector<Veci2> Steering::FeelerGridCollision(EnemyParent* enemy)
{
	std::vector<Veci2> container;
	int gridSize = enemy->GetGridSize();
	// get start and end positions of the feeler in grid position
	Vecf2 startPos = enemy->CalculateGridPosition(enemy->GetSpawnLocationOffset());
	Vecf2 endPos = enemy->CalculateGridPosition(enemy->GetSpawnLocationOffset() + enemy->currentVelocity * 2.0f);
	// calculate line segment
	float rise = endPos.y - startPos.y;
	float run = endPos.x - startPos.x;
	// if run != 0
	if (run != 0) {
		float gradient = rise / run;
		float intercept = startPos.y - gradient * startPos.x;
		// if run < gridCellDiameter, calculate points using rise
		float increment;
		int y;
		int x;
		// if dx and dy are both not smaller then gridCellDiameter
		//if (abs(run) > enemy->GetCellDiameter() && abs(rise) > enemy->GetCellDiameter()) {
		if (abs(rise) < abs(run)) {
			// calculate using run for more accuracy, also rise might be 0
			increment = startPos.x;
			// if run is positive
			if (run < 0) {
				while (increment > endPos.x) {
					increment -= 1.0f;
					// round y to int closest to start
					x = int(increment);
					// round x to int closest to point
					y = int((gradient*x + intercept) + 0.5f);
					if (x < gridSize && x > 0 && y < gridSize && y > 0) {
						container.emplace_back(Veci2(x, y));
					}
				}
			}
			else if (run > 0) {
				while (increment < endPos.x) {
					increment += 1.0f;
					// round y to int closest to start
					x = int(increment);
					// round x to int closest to point
					y = int((gradient*x + intercept) + 0.5f);
					if (x < gridSize && x > 0 && y < gridSize && y > 0) {
						container.emplace_back(Veci2(x, y));
					}
				}
			}
		}
		else if (abs(run) < abs(rise)) {
			// calculate using rise for more accuracy, and rise cannot be 0 in this case
			increment = startPos.y;
			// if rise is positive
			if (rise < 0) {
				while (increment > endPos.y) {
					increment -= 1.0f;
					// round y to int closest to start
					y = int(increment);
					// round x to int closest to point
					x = int(((y - intercept) / gradient) + 0.5f);
					if (x < gridSize && x > 0 && y < gridSize && y > 0) {
						container.emplace_back(Veci2(x, y));
					}
				}
			}
			else if (rise > 0) {
				while (increment < endPos.y) {
					increment += 1.0f;
					// round y to int closest to start
					y = int(increment);
					// round x to int closest to point
					x = int(((y - intercept) / gradient) + 0.5f);
					if (x < gridSize && x > 0 && y < gridSize && y > 0) {
						container.emplace_back(Veci2(x, y));
					}
				}
			}
		}
		else if (abs(run) == abs(rise)) {
			// calculate using run for more accuracy, in case rise is 0
			increment = startPos.y;
			// if rise is positive
			if (rise < 0) {
				while (increment > endPos.y) {
					increment -= 1.0f;
					// round y to int closest to start
					y = int(increment);
					// round x to int closest to point
					x = int(((y - intercept) / gradient) + 0.5f);
					if (x < gridSize && x > 0 && y < gridSize && y > 0) {
						container.emplace_back(Veci2(x, y));
					}
				}
			}
			else if (rise < 0) {
				while (increment < endPos.y) {
					increment += 1.0f;
					// round y to int closest to start
					y = int(increment);
					// round x to int closest to point
					x = int(((y - intercept) / gradient) + 0.5f);
					if (x < gridSize && x > 0 && y < gridSize && y > 0) {
						container.emplace_back(Veci2(x, y));
					}
				}
			}
		}
	}
	return container;
}

bool Steering::FeelerCollideSolid(EnemyParent* entity, const std::vector<Veci2>& feelerCells, NodeAStar*& node)
{
	for (Veci2 i : feelerCells) {
		NodeAStar* test = entity->terrainWithPath->GetGridCell(i);
		if (!test->GetWalkable()) {
			node = test;
			return true;
		}
	}
	return false;
}

bool Steering::FeelerCollideHeight(EnemyParent* entity, std::vector<Veci2>& feelerCells, NodeAStar*& node)
{
	float mark = entity->GetSpawnLocationOffset().y;
	if (feelerCells.size() > 5) {
		int i = 0;
		for (std::vector<Veci2>::iterator start = feelerCells.begin(); i < 5; std::advance(start, 1), i++) {
			NodeAStar* test = entity->terrainWithPath->GetGridCell(*start);
			if ((test->GetWorldPos().y - mark) > entity->terrainWithPath->GetCellDiameter()*2.0f) {
				node = test;
				return true;
			}
			mark = test->GetWorldPos().y;
		}
	}
	return false;
}

Vecf3 Steering::AvoidObstacles(const float & incomingMagnitude, const Vecf3 & centerPosition, const Vecf3 & collidePosition)
{
	// calculate outward vector
	Vecf3 outwardVector = (Vecf3(collidePosition.x - centerPosition.x, 0.0f, collidePosition.z - centerPosition.z)).GetNormalized();
	return outwardVector * incomingMagnitude;
}

Vecf3 Steering::AvoidWall(const Vecf3 & collisionPosition, const Vecf3& currentPosition)
{
	Vecf3 counterForce = (Vecf3(collisionPosition.x - currentPosition.x, 0.0f, collisionPosition.z - currentPosition.z)).GetNormalized();
	return counterForce * -1.0f;
}

Vecf3 Steering::Flocking(EnemyParent * entity)
{
	// used to calculate seperation
	Vecf3 seperationForce = {0.0f, 0.0f, 0.0f};
	// used to calculate alignment
	Vecf3 headingForce = { 0.0f, 0.0f, 0.0f };
	// used to calculate cohesion
	Vecf3 averagePosition = { 0.0f, 0.0f, 0.0f };
	int count = 0;
	std::vector<EnemyParent*> neighbourContainer = entity->entityQueryHandler->GetEnemiesWithinRange(entity);
	Vecf3 vectorToPosition;
	for (EnemyParent* e : neighbourContainer) {
		// calculating seperation force
		vectorToPosition = Vecf3(entity->GetSpawnLocationOffset().x - e->GetSpawnLocationOffset().x, 0.0f, entity->GetSpawnLocationOffset().z - e->GetSpawnLocationOffset().z);
		if (vectorToPosition.x > 0.1f || vectorToPosition.y > 0.1f || vectorToPosition.z > 0.1f) {
			seperationForce = seperationForce + (vectorToPosition.GetNormalized()) / vectorToPosition.Len();
		}
		// calculating heading force
		headingForce = headingForce + entity->headingVector;
		// calculating cohesion force, summing up positions
		averagePosition = averagePosition + entity->GetSpawnLocationOffset();

		count++;
	}
	// getting average heading
	if (count != 0) {
		headingForce = headingForce / (float)count;
		averagePosition = averagePosition / (float)count;
	}
	// setting vertical forces to 0
	headingForce.y = 0.0f;
	seperationForce.y = 0.0f;
	averagePosition.y = 0.0f;
	// getting cohesion steering force
	Vecf3 cohesionForce = Seek(entity, averagePosition);
	return seperationForce * 0.3f + headingForce * 0.01f + cohesionForce * 0.5f;
}

void Steering::ProcessFeelers(EnemyParent * entity)
{
	//FeelerGridCollision(entity)
}

Vecf3 Steering::CalculateSteering(EnemyParent * entity)
{
	float maxMagnitudeSq = sq(100.0f);
	Vecf3 seekingForce = Seek(entity);
	// calculate obstancle avoidance force
	std::vector<Veci2> feeler = FeelerGridCollision(entity);
	Vecf3 obstacleAvoid = { 0.0f, 0.0f, 0.0f };
	// if an obstacle is detected
	NodeAStar* node;
	if (FeelerCollideSolid(entity, feeler, node)) {
		obstacleAvoid = entity->steering.AvoidObstacles(1.0f, node->GetSolidCenter(), node->GetWorldPos());
	}
	// calculate height avoidance force
	NodeAStar* heightNode;
	Vecf3 wallAvoid = { 0.0f, 0.0f, 0.0f };
	if (FeelerCollideHeight(entity, feeler, heightNode)) {
		wallAvoid = entity->steering.AvoidWall(heightNode->GetWorldPos(), entity->GetSpawnLocationOffset());
		wallAvoid.y = 0.0f;
		wallAvoid = wallAvoid * 1.0f;
	}
	// calculate seperation force
	Vecf3 flockingForce = entity->steering.Flocking(entity);
	// calculate forces with priority, obstacleAvoid > wallAvoid > seekingForce > flockingForce
	Vecf3 steeringForce = { 0.0f, 0.0f, 0.0f };
	float steeringForceMagnitudeSq = steeringForce.LenSq();
	float surplusForce;
	if (steeringForceMagnitudeSq < maxMagnitudeSq) {
		// obstacleAvoid
		surplusForce = maxMagnitudeSq - steeringForceMagnitudeSq;
		if (obstacleAvoid.LenSq() < surplusForce) {
			steeringForce = steeringForce + obstacleAvoid;
		}
		else if (obstacleAvoid.x != 0.0f || obstacleAvoid.y != 0.0f || obstacleAvoid.z != 0.0f) {
			steeringForce = steeringForce + obstacleAvoid.GetNormalized() * surplusForce;
		}
		// wallAvoid
		steeringForceMagnitudeSq = steeringForce.LenSq();
		if (!(steeringForceMagnitudeSq < maxMagnitudeSq)) {
			return steeringForce;
		}
		surplusForce = maxMagnitudeSq - steeringForceMagnitudeSq;
		if (wallAvoid.LenSq() < surplusForce) {
			steeringForce = steeringForce + wallAvoid;
		}
		else if (wallAvoid.x != 0.0f || wallAvoid.y != 0.0f || wallAvoid.z != 0.0f) {
			steeringForce = steeringForce + wallAvoid.GetNormalized() * surplusForce;
		}
		// seekingForce
		steeringForceMagnitudeSq = steeringForce.LenSq();
		if (!(steeringForceMagnitudeSq < maxMagnitudeSq)) {
			return steeringForce;
		}
		surplusForce = maxMagnitudeSq - steeringForceMagnitudeSq;
		if (seekingForce.LenSq() < surplusForce) {
			steeringForce = steeringForce + seekingForce;
		}
		else if (seekingForce.x != 0.0f || seekingForce.y != 0.0f || seekingForce.z != 0.0f) {
			steeringForce = steeringForce + seekingForce.GetNormalized() * surplusForce;
		}
		// flockingForce
		steeringForceMagnitudeSq = steeringForce.LenSq();
		if (!(steeringForceMagnitudeSq < maxMagnitudeSq)) {
			return steeringForce;
		}
		surplusForce = maxMagnitudeSq - steeringForceMagnitudeSq;
		if (flockingForce.LenSq() < surplusForce) {
			steeringForce = steeringForce + flockingForce;
		}
		else if (flockingForce.x != 0.0f || flockingForce.y != 0.0f || flockingForce.z != 0.0f) {
			steeringForce = steeringForce + flockingForce.GetNormalized() * surplusForce;
		}
	}
	return steeringForce;
}
