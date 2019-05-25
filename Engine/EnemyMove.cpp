#include "EnemyMove.h"
#include "EnemyOne.h"

#include "EnemyIdle.h"

std::shared_ptr<EnemyMove> EnemyMove::Instance()
{
	static std::shared_ptr<EnemyMove> instance = std::make_shared<EnemyMove>();
	return instance;
}

void EnemyMove::Enter(EnemyOne *& entity)
{
	// enemyone id # has entered move state
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cmove /cstate.");
}

void EnemyMove::Execute(EnemyOne *& entity)
{
	// if entity has not reached destination, continue executing path
	if (entity->reachedDestination == false) {
		entity->ExecutePathSteering();
	}
	// else terminate path, and change state back to idle
	else {
		entity->TerminatePath();
		entity->stateMachine->ChangeState(EnemyIdle::Instance());
	}
}

void EnemyMove::Exit(EnemyOne *& entity)
{
	// enemyone id # has exited move state
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cmove /cstate.");
}