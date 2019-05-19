#pragma once

#include "State.h"

class TurretOne;
class TurretAttack : public State<TurretOne> {
public:
	TurretAttack()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<TurretAttack> Instance();
	virtual void Enter(TurretOne*& entity) override;
	virtual void Execute(TurretOne*& entity) override;
	virtual void Exit(TurretOne*& entity) override;
};