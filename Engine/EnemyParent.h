#pragma once

#include "Entity.h"

#include <assert.h>
#include <sstream>
#include <string>

class EnemyParent : public Entity {
public:
	EnemyParent(const float& size, const Vecf3& loc) 
		:
		Entity(size, loc)
	{
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) 
	{
		if (!needPath) {
			ExecutePath(stepCounter);
		}
		ChildUpdates(kbd, mouse, dt);
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw() = 0;
	virtual bool SetCurrentPath(const std::vector<Vecf3>& newPath) {
		currentPath = newPath;
		pathSize = (int)currentPath.size();
		InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " path /bfound.");
		InitStartPath();
		return true;
	}
	Vecf3 ApproximatePoints(const Vecf3& startPoint, const Vecf3& endPoint) {
		const float xStep = (endPoint.x - startPoint.x) * pathSpeed;
		const float yStep = (endPoint.y - startPoint.y) * pathSpeed;
		const float zStep = (endPoint.z - startPoint.z) * pathSpeed;
		return { xStep, yStep, zStep };
	}
	void InitStartPath() {
		assert(pathSize != 0);
		pathStep = 0;
		stepCounter = 0;
		currentPoint = currentPath[pathStep];
		nextPoint = currentPath[pathStep + 1];
		spawnLocationOffset = currentPath[pathStep];
		velBetweenPoints = ApproximatePoints(currentPoint, nextPoint);
		InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " path /binitialized.");
	}
	void ExecutePath(const int& stepCounterIn) {
		if (stepCounterIn < stepCounterMax) {
			spawnLocationOffset += velBetweenPoints;
			stepCounter++;
		}
		else if (pathStep < pathSize - 2) {
			pathStep++;
			currentPoint = nextPoint;
			nextPoint = currentPath[pathStep + 1];
			velBetweenPoints = ApproximatePoints(currentPoint, nextPoint);
			spawnLocationOffset = currentPoint;
			stepCounter = 0;
			InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " /bexecuting path step /y" + std::to_string(pathStep) + ".");
		}
		else {
			std::reverse(currentPath.begin(), currentPath.end());
			InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " path /breversed.");
			InitStartPath();
		}
	}
	void SetPathSpeed() {}
public:
	bool needPath = true;
	Vecf3 targetDestination = {-8.0f, 0.1f, -8.0f};
private:
	std::vector<Vecf3> currentPath;
	int pathSize = (int)currentPath.size();
	float pathSpeed = 0.05f;
	int stepCounter;
	int stepCounterMax = (int)(1 / pathSpeed);
	Vecf3 currentPoint;
	Vecf3 nextPoint;
	Vecf3 velBetweenPoints;
	int pathStep;

	std::wstringstream ss;
};
