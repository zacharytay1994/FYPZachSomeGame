#pragma once

#include "State.h"

class EnemyOne;

class EnemyAwaitPath : public State<EnemyOne> {
public:
	EnemyAwaitPath()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<EnemyAwaitPath> Instance();
	virtual void Enter(EnemyOne*& entity) override;
	virtual void Execute(EnemyOne*& entity) override;
	virtual void Exit(EnemyOne*& entity) override;
};