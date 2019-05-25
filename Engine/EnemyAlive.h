#pragma once

#include "State.h"

class EnemyOne;
class EnemyAlive : public State<EnemyOne> {
public:
	EnemyAlive()
		:
		State()
	{}
public:
	// singleton referencer
	static std::shared_ptr<EnemyAlive> Instance();
	virtual void Enter(EnemyOne*& entity) override;
	virtual void Execute(EnemyOne*& entity) override;
	virtual void Exit(EnemyOne*& entity) override;
};