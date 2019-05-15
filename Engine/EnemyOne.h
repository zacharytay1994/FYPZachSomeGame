#pragma once

#include "EnemyParent.h"

class EnemyOne : public EnemyParent {
public:
	EnemyOne(const float& size, const Vecf3& loc)
		:
		EnemyParent(size, loc)
	{
		InsertDebugString("/renemyone has been created.");
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
	virtual void Draw() override {}
private:
};