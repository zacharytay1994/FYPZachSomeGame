#pragma once

#include "Entity.h"
#include "Physics.h"

class ProjectileParent : public Entity {
public:
	ProjectileParent(const float& size, const Vecf3& loc, const Vecf3& velocity, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		Entity(size, loc, entityQueryHandler),
		velocity(velocity)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) {
		if (!stop) {
			// Update location with velocity
			spawnLocationOffset += velocity * dt;
			// change velocity due to gravitational acceleration
			velocity += gravitationalAcc * dt;
		}
		// extra updates specified by the child
		ChildUpdates(kbd, mouse, dt);
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void ChildMessage(const MessageDispatcher::Telegram& msg) override {}
	virtual void ApplyExternalForce(const Vecf3& force) {
		velocity += force;
	}
	virtual Vecf3 GetVelocity() {
		return velocity;
	}
public:
	bool toDestroy = false;
	bool stop = false;
	bool isColliding = false;
protected:
	Vecf3 velocity;
	const Vecf3 gravitationalAcc = { 0.0f, -9.81f, 0.0f };
};