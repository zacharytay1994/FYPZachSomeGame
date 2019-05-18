#pragma once

#include "Entity.h"
#include "BuildingParent.h"

#include <assert.h>
#include <sstream>
#include <string>

class TerrainWithPath;
class EnemyParent : public Entity {
public:
	EnemyParent(const float& size, const Vecf3& loc, std::shared_ptr<EntityQueryHandler>& entityQueryHandler, std::shared_ptr<TerrainWithPath>& terrainWithPath)
		:
		Entity(size, loc, entityQueryHandler),
		terrainWithPath(terrainWithPath)
	{}
	EnemyParent(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler, std::shared_ptr<TerrainWithPath>& terrainWithPath)
		:
		Entity(size, loc, heightDisplaced + size/4.0f, worldSize, gridSize, entityQueryHandler),
		terrainWithPath(terrainWithPath)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) 
	{
		// execute current state
		/*if (currentState) {
			currentState->Execute(this);
		}*/
		ChildUpdates(kbd, mouse, dt);
		/*if (reachedDestination == false) {
			ExecutePath(stepCounter);
		}
		else {
			TerminatePath();
			reachedDestination = true;
		}*/
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual bool SetCurrentPath(const std::vector<Vecf3>& newPath) {
		currentPath = newPath;
		pathSize = (int)currentPath.size();
		//InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " path /cfound.");
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
		//InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " path /cexecuted.");
	}
	void TerminatePath() {
		//pathStep = 0;
		//stepCounter = 0;
		currentPath.clear();
		notreversed = true;
		reachedDestination = false;
		InsertDebugString("/renemyone id: /y" + std::to_string(GetUniqueID()) + " /cpath terminated.");
	}
	void ExecutePath(const int& stepCounterIn) {
		// traverse edge
		if (stepCounterIn < stepCounterMax) {
			spawnLocationOffset += velBetweenPoints;
			stepCounter++;
		}
		// increment edge count, total edge = total points - 1
		else if (pathStep < pathSize - 2) {
			pathStep++;
			currentPoint = currentPath[pathStep];
			nextPoint = currentPath[pathStep + 1];
			velBetweenPoints = ApproximatePoints(currentPoint, nextPoint);
			spawnLocationOffset = currentPoint;
			stepCounter = 0;
			//InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " /bexecuting path step /y" + std::to_string(pathStep) + ".");
		}
		else if (notreversed) {
			std::reverse(currentPath.begin(), currentPath.end());
			InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " path /creversed.");
			InitStartPath();
			notreversed = false;
		}
		else {
			reachedDestination = true;
		}
	}
	int GetStepCounter() {
		return stepCounter;
	}
public:
	bool needPath = false;
	Vecf3 targetDestination = {-8.0f, 0.1f, -8.0f};
	bool reachedDestination = false;
	bool notreversed = true;

	Vecf3 originalSpawnLocation;

	// reference to query path
	std::shared_ptr<TerrainWithPath> terrainWithPath;
private:
	std::vector<Vecf3> currentPath;
	int pathSize = (int)currentPath.size();
	float pathSpeed = 0.05f;
	int stepCounterMax = (int)(1 / pathSpeed);
	Vecf3 currentPoint;
	Vecf3 nextPoint;
	Vecf3 velBetweenPoints;
	int pathStep;
	int stepCounter;

	std::wstringstream ss;

};
