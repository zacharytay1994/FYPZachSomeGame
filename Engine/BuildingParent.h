#pragma once

#include "Entity.h"

class BuildingParent : public Entity {
public:
	BuildingParent(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		Entity(size, loc, heightDisplaced + size/4.0f, worldSize, gridSize, entityQueryHandler)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt)
	{
		ChildUpdates(kbd, mouse, dt);
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0;

	virtual void ChildMessage(const MessageDispatcher::Telegram& msg) {
		switch (msg.enumMessage) {
			// MessageDispatcher::Messages::AttackFor1Damage
			case 0:
				health -= 1;
				InsertDebugString("/bbuildingone id: /y" + std::to_string(entityUniqueID) + " has lost /y1 health.");
				if (health <= 0) {
					isDestroyed = true;
					exists = false;
				}
				break;
		}
	}

	bool IsDestroyed() {
		return isDestroyed;
	}
public:
	int health = 3;
private:
	bool isDestroyed = false;
};