#pragma once

#include "Entity.h"
#include "BuildingParent.h"

#include <assert.h>
#include <sstream>
#include <string>

class TerrainWithPath;
class EnemyParent : public Entity {
public:
	EnemyParent(const float& size, const Vecf3& loc, std::shared_ptr<EntityQueryHandler>& entityQueryHandler, std::shared_ptr<TerrainWithPath>& terrainWithPath,
		const float& attackRange, const int& attackSpeed)
		:
		Entity(size, loc, entityQueryHandler),
		terrainWithPath(terrainWithPath),
		attackRange(attackRange),
		attackSpeed(attackSpeed)
	{}
	EnemyParent(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler, std::shared_ptr<TerrainWithPath>& terrainWithPath,
		const float& attackRange, const int& attackSpeed)
		:
		Entity(size, loc, heightDisplaced + size/4.0f, worldSize, gridSize, entityQueryHandler),
		terrainWithPath(terrainWithPath),
		attackRange(attackRange),
		attackSpeed(attackSpeed)
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
	virtual void ChildMessage(const MessageDispatcher::Telegram& msg) override {}
	virtual bool SetCurrentPath(const std::vector<Vecf3>& newPath) {
		currentPath = newPath;
		TrimPath(currentPath);
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
		// increment edge count, total edge = total points - 1, index = size - 1, hence pathSize - 2
		else if (pathStep < pathSize - 2) {
			pathStep++;
			currentPoint = currentPath[pathStep];
			nextPoint = currentPath[pathStep + 1];
			velBetweenPoints = ApproximatePoints(currentPoint, nextPoint);
			spawnLocationOffset = currentPoint;
			stepCounter = 0;
			//InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " /bexecuting path step /y" + std::to_string(pathStep) + ".");
		}
		/*else if (notreversed) {
			std::reverse(currentPath.begin(), currentPath.end());
			InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " path /creversed.");
			InitStartPath();
			notreversed = false;
		}*/
		else {
			reachedDestination = true;
		}
	}
	void TrimPath(std::vector<Vecf3>& currentPathIn) {
		float trimVal = 0.0f;
		std::vector<Vecf3>::iterator front = currentPathIn.begin();
		for (std::vector<Vecf3>::iterator back = (currentPathIn.end() - 1); (back != front) && (trimVal < (attackRange - cellDiameter)); std::advance(back, -1)) {
			trimVal += DistanceBetween(*back, *(back - 1));
			currentPathIn.pop_back();
		}
	}
	int GetStepCounter() {
		return stepCounter;
	}
	virtual bool AttackAtSpeed() {
		if (clock < timePerAttack) {
			clock += 1.0f / 60.0f;
			return true;
		}
		else {
			MessageDispatcher::Instance()->DispatchMsg(0.0, this, targetID, static_cast<int>(MessageDispatcher::Messages::AttackFor1Damage), nullptr);
			InsertDebugString("/renemyone id: /y" + std::to_string(entityUniqueID) + " attacked /bbuildingone id: /y" + std::to_string(targetID));
			clock = 0.0f;
			return true;
		}
	}
	bool IsDead() {
		return isDead;
	}
public:
	bool needPath = false;
	Vecf3 targetDestination = {-8.0f, 0.1f, -8.0f};
	bool reachedDestination = false;
	bool notreversed = true;
	// attack variables
	int targetID;
	float attackRange;
	int attackSpeed; 
	float clock = 0.0f;
	float timePerAttack = 1.0f / attackSpeed;
	int health = 3;
	bool isDead = false;

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
