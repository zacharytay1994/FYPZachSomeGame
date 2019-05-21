#include "TurretAttack.h"
#include "TurretOne.h"

#include "TurretIdle.h"

std::shared_ptr<TurretAttack> TurretAttack::Instance()
{
	static std::shared_ptr<TurretAttack> instance = std::make_shared<TurretAttack>();
	return instance;
}

void TurretAttack::Enter(TurretOne *& entity)
{
	entity->InsertDebugString("/tturretone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cattack /cstate.");
}

void TurretAttack::Execute(TurretOne *& entity)
{
	// update target location with target entity location
	entity->targetLocation = entity->target->GetSpawnLocationOffset();
	// if target is within firing range, and target exists, i.e. alive, calls attack
	if (!(entity->DistanceBetween(entity->GetSpawnLocationOffset(), entity->targetLocation) > entity->maxRange) && entity->target->exists) {
		entity->Attack();
	}
	// else set targetFound flag to false, and switch state back to idle state
	else {
		entity->targetFound = false;
		entity->stateMachine->ChangeState(TurretIdle::Instance());
	}
}

void TurretAttack::Exit(TurretOne *& entity)
{
	entity->InsertDebugString("/tturretone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cattack /cstate.");
}