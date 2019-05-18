#include "EnemyAwaitPath.h"
#include "EnemyOne.h"

#include "TerrainWithPath.h"

#include "EnemyMove.h"

std::shared_ptr<EnemyAwaitPath> EnemyAwaitPath::Instance()
{
	static std::shared_ptr<EnemyAwaitPath> instance = std::make_shared<EnemyAwaitPath>();
	return instance;
}

void EnemyAwaitPath::Enter(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cawaiting /cpath /cstate.");
}

void EnemyAwaitPath::Execute(EnemyOne *& entity)
{
	//entity->InsertDebugString("now executing /cawaiting /cpath /c state.");
	// attempt to find path to target destination
	std::vector<Vecf3> holder;
	if (entity->terrainWithPath->FindAndReturnPath(entity->GetSpawnLocationOffset(), entity->targetDestination, holder)) {
		entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " /cpath /cfound.");
		entity->SetCurrentPath(holder);
		entity->stateMachine->ChangeState(EnemyMove::Instance());
	}
}

void EnemyAwaitPath::Exit(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cawaiting /cpath /cstate.");
}