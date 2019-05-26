#include "EnemyParent.h"
#include "TerrainWithPath.h"

void EnemyParent::Update(Keyboard & kbd, Mouse & mouse, float dt)
{
	// steering updates
	spawnLocationOffset = spawnLocationOffset + (currentVelocity + gravitationalAcceleration) * dt;
	if (currentVelocity.x > 0.1f || currentVelocity.y > 0.1f || currentVelocity.z > 0.1f) {
		headingVector = currentVelocity.GetNormalized();
	}
	currentVelocity = currentVelocity + steering.CalculateSteering(this);
	terrainWithPath->QueryQuadCollisionEstimateEnemy(spawnLocationOffset, this);
	// child updates
	ChildUpdates(kbd, mouse, dt);
}
