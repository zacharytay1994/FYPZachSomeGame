#pragma once

#include "Entity.h"
#include "EnemyParent.h"

#include <memory>

template <class EntityType>
class State {
public:
	State() {}
	virtual void Enter(EntityType* entity) = 0;
	virtual void Execute(EntityType* entity) = 0;
	virtual void Exit(EntityType* entity) = 0;
private:
};