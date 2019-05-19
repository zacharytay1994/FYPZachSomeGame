#include "BuildingAlive.h"
#include "BuildingOne.h"

#include "BuildingDead.h"

std::shared_ptr<BuildingAlive> BuildingAlive::Instance()
{
	static std::shared_ptr<BuildingAlive> instance = std::make_shared<BuildingAlive>();
	return instance;
}

void BuildingAlive::Enter(BuildingOne *& entity)
{
	entity->InsertDebugString("/rbuildingone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cglobal /calive /cstate.");
}

void BuildingAlive::Execute(BuildingOne *& entity)
{
	// if health <= 0, change to global state dead
	if (entity->health <= 0) {
		entity->stateMachine->ChangeGlobalState(BuildingDead::Instance());
	}
}

void BuildingAlive::Exit(BuildingOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cglobal /calive /cstate.");
}