#pragma once

#include "Entity.h"

#include <assert.h>

class EnemyParent : public Entity {
public:
	EnemyParent(const float& size, const Vecf3& loc) 
		:
		Entity(size, loc)
	{}
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
		pathSize = currentPath.size();
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
	}
	void ExecutePath(const float& stepCounterIn) {
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
		}
		else {
			std::reverse(currentPath.begin(), currentPath.end());
			InitStartPath();
		}
	}
	void SetPathSpeed() {}
public:
	bool needPath = true;
	Vecf3 targetDestination = {-9.8f, 0.1f, -8.0f};
private:
	std::vector<Vecf3> currentPath;
	int pathSize = currentPath.size();
	float pathSpeed = 0.2f;
	int stepCounter;
	int stepCounterMax = (int)(1 / pathSpeed);
	Vecf3 currentPoint;
	Vecf3 nextPoint;
	Vecf3 velBetweenPoints;
	int pathStep;
};
