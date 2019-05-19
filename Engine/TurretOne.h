#pragma once

#include "TurretParent.h"
#include "Physics.h"
#include "StateMachine.h"

#include "TurretIdle.h"

class TurretOne : public TurretParent {
public:
	TurretOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, const int& rateOfFire, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		TurretParent(size, loc, heightDisplaced, worldSize, gridSize, rateOfFire, entityQueryHandler),
		stateMachine(std::make_unique<StateMachine<TurretOne>>(this))
	{
		InsertDebugString("/tturretone /y" + std::to_string(entityUniqueID) + " has been /ccreated.");
		stateMachine->SetCurrentState(TurretIdle::Instance());
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		stateMachine->Update();
	}
	void Attack() {
		FireAtRate(ProjectileType::ProjectileOne);
	}
public:
	std::unique_ptr<StateMachine<TurretOne>> stateMachine;
};