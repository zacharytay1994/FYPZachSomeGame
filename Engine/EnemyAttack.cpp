#include "EnemyAttack.h"
#include "EnemyOne.h"

#include "EnemyIdle.h"

std::shared_ptr<EnemyAttack> EnemyAttack::Instance()
{
	static std::shared_ptr<EnemyAttack> instance = std::make_shared<EnemyAttack>();
	return instance;
}

void EnemyAttack::Enter(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cattack /cstate.");
}

void EnemyAttack::Execute(EnemyOne *& entity)
{
	// if entity of targetID still exists, i.e. is alive, calls attack on it
	if (entity->entityQueryHandler->GetEntityFromID(entity->targetID)->exists) {
		entity->AttackAtSpeed();
	}
	// else changes state back to idle
	else {
		entity->stateMachine->ChangeState(EnemyIdle::Instance());
	}
}

void EnemyAttack::Exit(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cattack /cstate.");
}
