#pragma once

#include "EnemyParent.h"
#include "StateMachine.h"
#include "EnemyIdle.h"
#include "EnemyAwaitPath.h"

class EnemyOne : public EnemyParent {
public:
	EnemyOne(const float& size, const Vecf3& loc)
		:
		EnemyParent(size, loc),
		stateMachine(std::make_unique<StateMachine<EnemyOne>>(this))
	{
		//entityPipeline->effect.pixelShader.BindTexture("greenimage.bmp");
		InsertDebugString("/renemyone has been created");
	}
	EnemyOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize)
		:
		EnemyParent(size, loc, heightDisplaced, worldSize, gridSize),
		stateMachine(std::make_unique<StateMachine<EnemyOne>>(this))
	{
		InsertDebugString("/renemyone /y" + std::to_string(entityUniqueID) + " has been /ccreated.");
		stateMachine->SetCurrentState(EnemyIdle::Instance());
		InsertDebugString("/renemy /y" + std::to_string(entityUniqueID) + " /cstate set.");
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {
		stateMachine->Update();
	}
public:
	std::unique_ptr<StateMachine<EnemyOne>> stateMachine;
};