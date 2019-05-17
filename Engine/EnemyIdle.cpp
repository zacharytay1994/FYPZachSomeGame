#include "EnemyIdle.h"

std::shared_ptr<EnemyIdle> EnemyIdle::Instance()
{
	static std::shared_ptr<EnemyIdle> instance;
	return instance;
}

void EnemyIdle::Enter(EnemyParent * entity)
{
}

void EnemyIdle::Execute(EnemyParent * entity)
{
}

void EnemyIdle::Exit(EnemyParent * entity)
{
}
