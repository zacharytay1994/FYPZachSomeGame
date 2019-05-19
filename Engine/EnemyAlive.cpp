#include "EnemyAlive.h"
#include "EnemyOne.h"

#include "EnemyDead.h"

std::shared_ptr<EnemyAlive> EnemyAlive::Instance()
{
	static std::shared_ptr<EnemyAlive> instance = std::make_shared<EnemyAlive>();
	return instance;
}

void EnemyAlive::Enter(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cglobal /calive /cstate.");
}

void EnemyAlive::Execute(EnemyOne *& entity)
{
	if (entity->health > 0) {
		int holder;
		if (holder = entity->entityQueryHandler->CheckProjectileEnemyCollision(entity) > 0) {
			entity->health -= holder;
			entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " has lost /y" + std::to_string(holder) + " health.");
			entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " has /y" + std::to_string(entity->health) + " left.");
		}
	}
	else {
		entity->isDead = true;
		entity->exists = false;
		entity->stateMachine->ChangeGlobalState(EnemyDead::Instance());
	}
}

void EnemyAlive::Exit(EnemyOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cglobal /calive /cstate.");
}
