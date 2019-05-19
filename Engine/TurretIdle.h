#pragma once

#include "State.h"

class TurretOne;
class TurretIdle : public State<TurretOne> {
public:
	TurretIdle()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<TurretIdle> Instance();
	virtual void Enter(TurretOne*& entity) override;
	virtual void Execute(TurretOne*& entity) override;
	virtual void Exit(TurretOne*& entity) override;
};