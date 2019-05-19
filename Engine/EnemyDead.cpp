#include "EnemyDead.h"
#include "EnemyOne.h"

std::shared_ptr<EnemyDead> EnemyDead::Instance()
{
	static std::shared_ptr<EnemyDead> instance = std::make_shared<EnemyDead>();
	return instance;
}

void EnemyDead::Enter(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cglobal /cdead /cstate.");
}

void EnemyDead::Execute(EnemyOne *& entity)
{
	
}

void EnemyDead::Exit(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cglobal /cdead /cstate.");
}