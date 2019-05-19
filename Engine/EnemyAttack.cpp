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
	//MessageDispatcher::Instance()->DispatchMsg(0.0, entity, entity->targetID, 1, nullptr);
	if (!entity->entityQueryHandler->GetEntityFromID(entity->targetID)->UnableToReceiveMessage()) {
		entity->AttackAtSpeed();
	}
	else {
		entity->stateMachine->ChangeState(EnemyIdle::Instance());
	}
}

void EnemyAttack::Exit(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cattack /cstate.");
}
