#pragma once

#include "EnemyParent.h"
#include "StateMachine.h"
#include "EnemyIdle.h"
#include "EnemyAwaitPath.h"
#include "EnemyAlive.h"

class EnemyOne : public EnemyParent {
public:
	EnemyOne(const float& size, const Vecf3& loc, std::shared_ptr<EntityQueryHandler>& entityQueryHandler, std::shared_ptr<TerrainWithPath>& terrainWithPath)
		:
		EnemyParent(size, loc, entityQueryHandler, terrainWithPath, 5.0f, 1),
		stateMachine(std::make_unique<StateMachine<EnemyOne>>(this))
	{
		//entityPipeline->effect.pixelShader.BindTexture("greenimage.bmp");
		InsertDebugString("/renemyone id: /y" + std::to_string(entityUniqueID) + " has been /ccreated");
	}
	EnemyOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler, std::shared_ptr<TerrainWithPath>& terrainWithPath)
		:
		EnemyParent(size, loc, heightDisplaced, worldSize, gridSize, entityQueryHandler, terrainWithPath, 5.0f, 1),
		stateMachine(std::make_unique<StateMachine<EnemyOne>>(this))
	{
		InsertDebugString("/renemyone id: /y" + std::to_string(entityUniqueID) + " has been /ccreated.");
		//MessageDispatcher::Instance()->DispatchMsg(0.0, this, 0, 1, nullptr);
		stateMachine->SetCurrentState(EnemyIdle::Instance());
		stateMachine->SetGlobalState(EnemyAlive::Instance());
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {
		stateMachine->Update();
	}
	void InitializeMessage() {
		InsertDebugString("/renemyone id: /y" + std::to_string(entityUniqueID) + " has been /ccreated.");
	}
public:
	std::unique_ptr<StateMachine<EnemyOne>> stateMachine;
};