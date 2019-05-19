#pragma once

#include "State.h"

class EnemyOne;
class EnemyDead : public State<EnemyOne> {
public:
	EnemyDead()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<EnemyDead> Instance();
	virtual void Enter(EnemyOne*& entity) override;
	virtual void Execute(EnemyOne*& entity) override;
	virtual void Exit(EnemyOne*& entity) override;
};