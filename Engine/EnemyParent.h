#pragma once

#include "Entity.h"

class EnemyParent : public Entity {
public:
	EnemyParent(const float& size, const Vecf3& loc) 
		:
		Entity(size, loc)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) 
	{
		ChildUpdates(kbd, mouse, dt);
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw() = 0;
	virtual bool SetCurrentPath(const std::vector<Vecf3>& newPath) {
		currentPath = newPath;
	}
public:
	bool needPath = true;
	Vecf3 targetDestination;
private:
	std::vector<Vecf3> currentPath;
};
