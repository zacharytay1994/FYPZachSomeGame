#pragma once

#include "State.h"

class BuildingOne;
class BuildingAlive : public State<BuildingOne> {
public:
	BuildingAlive()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<BuildingAlive> Instance();
	virtual void Enter(BuildingOne*& entity) override;
	virtual void Execute(BuildingOne*& entity) override;
	virtual void Exit(BuildingOne*& entity) override;
};