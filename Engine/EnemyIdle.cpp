#include "EnemyIdle.h"
#include "EnemyOne.h"

#include "EnemyAwaitPath.h"
#include "EnemyAttack.h"

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
	// if target found
	Vecf3 targetDestination; float distanceAway; int targetID;
	if (entity->entityQueryHandler->QueryNearestBuilding(entity->GetSpawnLocationOffset(), targetDestination, distanceAway, targetID)) {
		entity->targetDestination = targetDestination;
		entity->targetID = targetID;
		entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " target found.");
		// if target out of attack range, change state to await path
		if (distanceAway > sq(entity->attackRange)) {
			entity->stateMachine->ChangeState(EnemyAwaitPath::Instance());
		}
		// if target within attack range, change state to attack
		else {
			entity->stateMachine->ChangeState(EnemyAttack::Instance());
		}
	}
}

void EnemyIdle::Exit(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cidle /cstate.");
}
