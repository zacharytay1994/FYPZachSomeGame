#pragma once

#include "Entity.h"
#include "BuildingParent.h"
#include "Steering.h"

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
		Entity(size, loc, heightDisplaced + size/2.0f, worldSize, gridSize, entityQueryHandler),
		terrainWithPath(terrainWithPath),
		attackRange(attackRange),
		attackSpeed(attackSpeed)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt);
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void ChildMessage(const MessageDispatcher::Telegram& msg) override {}
	// elaborate set method to set current path, together with other relavant attributes, also trims the path
	virtual bool SetCurrentPath(const std::vector<Vecf3>& newPath) {
		currentPath = newPath;
		TrimPath(currentPath);
		pathSize = (int)currentPath.size();
		InitStartPath();
		return true;
	}
	// calculates the velocity required to traverse 2 points at a particular speed (in this case pathSpeed)
	Vecf3 ApproximatePoints(const Vecf3& startPoint, const Vecf3& endPoint) {
		const float xStep = (endPoint.x - startPoint.x) * pathSpeed;
		const float yStep = (endPoint.y - startPoint.y) * pathSpeed;
		const float zStep = (endPoint.z - startPoint.z) * pathSpeed;
		return { xStep, yStep, zStep };
	}
	// intializes the current path by setting and calculating various variables to be used while executing path
	void InitStartPath() {
		assert(pathSize != 0);
		pathStep = 0;
		stepCounter = 0;
		currentPoint = currentPath[pathStep];
		autonomousAgentDestination = currentPoint;
		nextPoint = currentPath[pathStep + 1];
		spawnLocationOffset = currentPath[pathStep];
		velBetweenPoints = ApproximatePoints(currentPoint, nextPoint);
	}
	// clears currentPath array, and sets reachedDestination flag back to false
	void TerminatePath() {
		currentPath.clear();
		reachedDestination = false;
		InsertDebugString("/renemyone id: /y" + std::to_string(GetUniqueID()) + " /cpath terminated.");
	}
	// executed the path, following the Vecf3 point array
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
		}
		// if reached the second last elements of currentPath array, i.e. end of path
		else {
			reachedDestination = true;
		}
	}
	void ExecutePathSteering() {
		if (currentPath.size() != 0) {
			if (pathStep < pathSize - 1) {
				if ((currentPoint - spawnLocationOffset).LenSq() < sq(wayPointRange)) {
					pathStep++;
					currentPoint = currentPath[pathStep];
					autonomousAgentDestination = currentPoint;
				}
			}
			else {
				reachedDestination = true;
			}
		}
	}
	// shortens the path (Vecf3 point array) to stop once within attack range
	void TrimPath(std::vector<Vecf3>& currentPathIn) {
		float trimVal = 0.0f;
		std::vector<Vecf3>::iterator front = currentPathIn.begin();
		for (std::vector<Vecf3>::iterator back = (currentPathIn.end() - 1); (back != front) && (trimVal < (attackRange - cellDiameter)); std::advance(back, -1)) {
			trimVal += DistanceBetween(*back, *(back - 1));
			currentPathIn.pop_back();
		}
	}
	// attacks at speed of attackSpeed/second
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
	virtual void ApplyExternalForce(const Vecf3& force) {
		currentVelocity += force;
	}
	int GetStepCounter() {
		return stepCounter;
	}
	bool IsDead() {
		return isDead;
	}
public:
	// path flags
	// needPath not really used, but just leaving it in case for the future
	bool	needPath = false;
	// target destination
	Vecf3	targetDestination;
	bool	reachedDestination = false;

	// attack variables
	// entityUniqueID of current entity target
	int		targetID;
	float	attackRange;
	int		attackSpeed; 
	float	clock = 0.0f;
	float	timePerAttack = 1.0f / attackSpeed;
	int		health = 3;

	// global flag to whether enemy is dead
	bool	isDead = false;

	// steering variables
	float speed = 12.0f;
	float turningForce = 0.5f;
	Vecf3 currentVelocity = Vecf3(0.0f, 0.0f, 0.0f);
	Vecf3 autonomousAgentDestination = Vecf3(0.0f, 0.0f, 0.0f);
	Vecf3 headingVector = Vecf3(0.0f, 0.0f, 0.0f); 
	Vecf3 gravitationalAcceleration = { 0.0f, -9.81f, 0.0f };
	float wayPointRange = 5.0f;
	Steering steering;
	Entity* targetEntity;

	// reference to query path
	std::shared_ptr<TerrainWithPath> terrainWithPath;
private:
	std::vector<Vecf3> currentPath;
	int		pathSize = (int)currentPath.size();
	float	pathSpeed = 0.1f;
	int		stepCounterMax = (int)(1 / pathSpeed);
	int		pathStep;
	int		stepCounter;
	Vecf3	currentPoint;
	Vecf3	nextPoint;
	Vecf3	velBetweenPoints;

	std::wstringstream ss;
};
