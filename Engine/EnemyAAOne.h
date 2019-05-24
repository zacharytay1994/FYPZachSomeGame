#pragma once

#include "EnemyParent.h"
#include "StateMachine.h"
#include "Steering.h"

#include <vector>

class EnemyAAOne : public EnemyParent {
public:
	EnemyAAOne(const float& size, const Vecf3& loc, std::shared_ptr<EntityQueryHandler>& entityQueryHandler, std::shared_ptr<TerrainWithPath>& terrainWithPath)
		:
		EnemyParent(size, loc, entityQueryHandler, terrainWithPath, 5.0f, 1), // attackRange, attackSpeed
		stateMachine(std::make_unique<StateMachine<EnemyAAOne>>(this))
	{
		InsertDebugString("/renemyone id: /y" + std::to_string(entityUniqueID) + " has been /ccreated");
	}
	EnemyAAOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler, std::shared_ptr<TerrainWithPath>& terrainWithPath)
		:
		EnemyParent(size, loc, heightDisplaced, worldSize, gridSize, entityQueryHandler, terrainWithPath, 5.0f, 1),
		stateMachine(std::make_unique<StateMachine<EnemyAAOne>>(this))
	{
		InsertDebugString("/renemyone id: /y" + std::to_string(entityUniqueID) + " has been /ccreated.");
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {
		spawnLocationOffset = spawnLocationOffset + currentVelocity * dt;
		currentVelocity = currentVelocity + steering.CalculateSteering(this);
	}
	void InitializeMessage() {
		InsertDebugString("/renemyone id: /y" + std::to_string(entityUniqueID) + " has been /ccreated.");
	}
public:
	std::unique_ptr<StateMachine<EnemyAAOne>> stateMachine;
	float speed = 8.0f;
	float turningForce = 0.15f;
	Vecf3 currentVelocity = Vecf3( 0.0f, 0.0f, 0.0f ) * speed;
	Vecf3 targetDestination = { 0.0f, 0.0f, 0.0f };
	Steering steering;
	// test container
	std::vector<Veci2> container;
};