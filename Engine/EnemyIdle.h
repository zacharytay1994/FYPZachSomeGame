#pragma once

#include "State.h"

class EnemyIdle : public State<EnemyParent> {
private:
	EnemyIdle()
		:
		State()
	{}
public:
	// singleton reference
	static std::shared_ptr<EnemyIdle> Instance();
	virtual void Enter(EnemyParent* entity) override;
	virtual void Execute(EnemyParent* entity) override;
	virtual void Exit(EnemyParent* entity) override;
};