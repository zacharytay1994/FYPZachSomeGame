#include "EnemyAwaitPath.h"
#include "EnemyOne.h"

std::shared_ptr<EnemyAwaitPath> EnemyAwaitPath::Instance()
{
	static std::shared_ptr<EnemyAwaitPath> instance = std::make_shared<EnemyAwaitPath>();
	return instance;
}

void EnemyAwaitPath::Enter(EnemyOne *& entity)
{
	entity->InsertDebugString("enemyone /y" + std::to_string(entity->GetUniqueID()) + " entered /cawaiting /cpath /cstate.");
}

void EnemyAwaitPath::Execute(EnemyOne *& entity)
{
	//entity->InsertDebugString("now executing /cawaiting /cpath /c state.");
}

void EnemyAwaitPath::Exit(EnemyOne *& entity)
{
}