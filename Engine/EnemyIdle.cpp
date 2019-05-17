#include "EnemyIdle.h"
#include "EnemyOne.h"

#include "EnemyAwaitPath.h"

//template<class EnemyType>
//std::shared_ptr<EnemyIdle<EnemyType>> EnemyIdle<EnemyType>::Instance()
//{
//	static std::shared_ptr<EnemyIdle<EnemyType>> instance = std::make_shared<EnemyIdle<EnemyType>>();
//	return instance;
//}

std::shared_ptr<EnemyIdle> EnemyIdle::Instance()
{
	static std::shared_ptr<EnemyIdle> instance = std::make_shared<EnemyIdle>();
	return instance;
}

void EnemyIdle::Enter(EnemyOne *& entity)
{
	entity->InsertDebugString("enemyone /y" + std::to_string(entity->GetUniqueID()) + " entered /cidle /cstate.");
}

void EnemyIdle::Execute(EnemyOne *& entity)
{
	//entity->InsertDebugString("now executing /cidle /cstate.");
	if (entity->needPath) {
		entity->stateMachine->ChangeState(EnemyAwaitPath::Instance());
	}
}

void EnemyIdle::Exit(EnemyOne *& entity)
{
	entity->InsertDebugString("enemyone /y" + std::to_string(entity->GetUniqueID()) + " exited /cidle /cstate.");
}
