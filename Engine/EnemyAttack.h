#pragma once

#include "State.h"

class EnemyOne;
class EnemyAttack : public State<EnemyOne> {
public:
	EnemyAttack()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<EnemyAttack> Instance();
	virtual void Enter(EnemyOne*& entity) override;
	virtual void Execute(EnemyOne*& entity) override;
	virtual void Exit(EnemyOne*& entity) override;
};