#pragma once

#include "BuildingParent.h"
#include "StateMachine.h"

#include "BuildingAlive.h"

class BuildingOne : public BuildingParent {
public:
	BuildingOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		BuildingParent(size, loc, heightDisplaced, worldSize, gridSize, entityQueryHandler),
		stateMachine(std::make_unique<StateMachine<BuildingOne>>(this))
	{
		InsertDebugString("/bbuildingone id: /y" + std::to_string(entityUniqueID) + " has been /ccreated.");
		stateMachine->SetGlobalState(BuildingAlive::Instance());
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {
		stateMachine->Update();
	}
public:
	std::unique_ptr<StateMachine<BuildingOne>> stateMachine;
};