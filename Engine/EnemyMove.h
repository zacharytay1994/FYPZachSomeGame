#pragma once

#include "State.h"

class EnemyOne;
class EnemyMove : public State<EnemyOne> {
public:
	EnemyMove()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<EnemyMove> Instance();
	virtual void Enter(EnemyOne*& entity) override;
	virtual void Execute(EnemyOne*& entity) override;
	virtual void Exit(EnemyOne*& entity) override;
};