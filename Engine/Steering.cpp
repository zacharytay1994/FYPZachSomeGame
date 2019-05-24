#include "Steering.h"

#include "EnemyAAOne.h"
#include "TerrainWithPath.h"

Vecf3 Steering::Seek(EnemyAAOne* entity)
{
	// calculate desired velocity
	Vecf3 desiredVelocity = (entity->autonomousAgentDestination - entity->GetSpawnLocationOffset()).GetNormalized() * entity->speed;
	// get and return steering velocity
	Vecf3 steeringForce = (desiredVelocity - entity->currentVelocity).GetNormalized();
	// limit the steering force
	return steeringForce * entity->turningForce;
}

std::vector<Veci2> Steering::FeelerGridCollision(EnemyAAOne* enemy)
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

bool Steering::FeelerCollideSolid(EnemyAAOne* entity, const std::vector<Veci2>& feelerCells, NodeAStar*& node)
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

Vecf3 Steering::AvoidObstacles(const float & incomingMagnitude, const Vecf3 & centerPosition, const Vecf3 & collidePosition)
{
	// calculate outward vector
	Vecf3 outwardVector = (Vecf3(collidePosition.x - centerPosition.x, 0.0f, collidePosition.z - centerPosition.z)).GetNormalized();
	return outwardVector * incomingMagnitude;
}

Vecf3 Steering::Seperation(EnemyAAOne * entity)
{
	Vecf3 seperationForce = {0.0f, 0.0f, 0.0f};
	Vecf3 headingForce = { 0.0f, 0.0f, 0.0f };
	int count = 0;
	std::vector<EnemyParent*> neighbourContainer = entity->entityQueryHandler->GetEnemiesWithinRange(entity);
	Vecf3 vectorToPosition;
	for (EnemyParent* e : neighbourContainer) {
		vectorToPosition = Vecf3(entity->GetSpawnLocationOffset().x - e->GetSpawnLocationOffset().x, 0.0f, entity->GetSpawnLocationOffset().z - e->GetSpawnLocationOffset().z);
		if (vectorToPosition.x > 0.1f || vectorToPosition.y > 0.1f || vectorToPosition.z > 0.1f) {
			seperationForce = seperationForce + (vectorToPosition.GetNormalized()) / vectorToPosition.Len();
		}
		headingForce = headingForce + entity->headingVector;
		count++;
	}
	if (count != 0) {
		headingForce = headingForce / (float)count;
	}
	headingForce.y = 0.0f;
	seperationForce.y = 0.0f;
	return seperationForce * 0.3f + headingForce * 0.0f;
}

void Steering::ProcessFeelers(EnemyAAOne * entity)
{
	//FeelerGridCollision(entity)
}

Vecf3 Steering::CalculateSteering(EnemyAAOne * entity)
{
	Vecf3 seekingForce = Seek(entity);
	// calculate obstancle avoidance force
	Vecf3 obstacleAvoid = { 0.0f, 0.0f, 0.0f };
	// if an obstacle is detected
	NodeAStar* node;
	if (FeelerCollideSolid(entity, FeelerGridCollision(entity), node)) {
		obstacleAvoid = entity->steering.AvoidObstacles(1.0f, node->GetSolidCenter(), node->GetWorldPos());
	}
	// calculate seperation force
	Vecf3 flockingForce = entity->steering.Seperation(entity);
	
	return seekingForce + obstacleAvoid + flockingForce;
}
