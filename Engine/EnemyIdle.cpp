#include "EnemyIdle.h"
#include "EnemyOne.h"

#include "EnemyAwaitPath.h"

std::shared_ptr<EnemyIdle> EnemyIdle::Instance()
{
	static std::shared_ptr<EnemyIdle> instance = std::make_shared<EnemyIdle>();
	return instance;
}

void EnemyIdle::Enter(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cidle /cstate.");
}

void EnemyIdle::Execute(EnemyOne *& entity)
{
	// if target found(far)
	Vecf3 target;
	if (entity->entityQueryHandler->QueryNearestBuilding(entity->GetSpawnLocationOffset(), target)) {
		entity->targetDestination = target;
		entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " target found.");
		entity->needPath = true;
		entity->stateMachine->ChangeState(EnemyAwaitPath::Instance());
	}
}

void EnemyIdle::Exit(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cidle /cstate.");
}
