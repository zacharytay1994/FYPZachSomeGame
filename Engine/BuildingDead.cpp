#include "BuildingDead.h"
#include "BuildingOne.h"

std::shared_ptr<BuildingDead> BuildingDead::Instance()
{
	static std::shared_ptr<BuildingDead> instance = std::make_shared<BuildingDead>();
	return instance;
}

void BuildingDead::Enter(BuildingOne *& entity)
{
	entity->InsertDebugString("/rbuildingone id: /y" + std::to_string(entity->GetUniqueID()) + " entered /cglobal /cdead /cstate.");
}

void BuildingDead::Execute(BuildingOne *& entity)
{
}

void BuildingDead::Exit(BuildingOne *& entity)
{
	entity->InsertDebugString("/renemyone id: /y" + std::to_string(entity->GetUniqueID()) + " exited /cglobal /cdead /cstate.");
}