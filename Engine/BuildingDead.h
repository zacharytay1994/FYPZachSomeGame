#pragma once

#include "State.h"

class BuildingOne;
class BuildingDead : public State<BuildingOne> {
public:
	BuildingDead()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<BuildingDead> Instance();
	virtual void Enter(BuildingOne*& entity) override;
	virtual void Execute(BuildingOne*& entity) override;
	virtual void Exit(BuildingOne*& entity) override;
};