#pragma once

#include "EnemyParent.h"

class EnemyOne : public EnemyParent {
public:
	EnemyOne(const float& size, const Vecf3& loc)
		:
		EnemyParent(size, loc)
	{
		//entityPipeline->effect.pixelShader.BindTexture("greenimage.bmp");
		InsertDebugString("/renemyone has been created.");
	}
	EnemyOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize)
		:
		EnemyParent(size, loc, heightDisplaced, worldSize, gridSize)
	{
		InsertDebugString("/renemyone /y" + std::to_string(entityUniqueID) + " has been /ccreated.");
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
private:
};