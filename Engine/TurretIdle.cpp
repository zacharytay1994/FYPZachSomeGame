#include "TurretIdle.h"
#include "TurretOne.h"

#include "TurretAttack.h"

std::shared_ptr<TurretIdle> TurretIdle::Instance()
{
	static std::shared_ptr<TurretIdle> instance = std::make_shared<TurretIdle>();
	return instance;
}

void TurretIdle::Enter(TurretOne *& entity)
{
	entity->InsertDebugString("/tturretone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cidle /cstate.");
}

void TurretIdle::Execute(TurretOne *& entity)
{
	// if target found
	if (entity->entityQueryHandler->QueryNearestEnemy(entity->GetSpawnLocationOffset(), entity->target)) {
		// if within range
		if (entity->DistanceBetween(entity->GetSpawnLocationOffset(), entity->target->GetSpawnLocationOffset()) < entity->maxRange) {
			entity->targetFound = true;
				entity->stateMachine->ChangeState(TurretAttack::Instance());
		}
	}
}

void TurretIdle::Exit(TurretOne *& entity)
{
	entity->InsertDebugString("/tturretone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cidle /cstate.");
}