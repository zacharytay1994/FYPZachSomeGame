#pragma once

#include "Entity.h"
#include "Physics.h"

class ProjectileParent : public Entity {
public:
	enum class ProjectionType {
		low,
		lob
	};
public:
	ProjectileParent(const float& size, const Vecf3& loc, const Vecf3& targetLocation, const float& projectileSpeed,
		const ProjectionType& projectionType)
		:
		Entity(size, loc),
		targetLocation(targetLocation),
		projectileSpeed(projectileSpeed)
	{
		SetVelocity(static_cast<int>(projectionType));
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) {
		// Update location with velocity
		spawnLocationOffset += velocity * dt;
		// change velocity due to gravitational acceleration
		velocity += gravitationalAcc * dt;

		// extra updates specified by the child
		ChildUpdates(kbd, mouse, dt);
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw() = 0;
	virtual bool CalculateAngleOfProjection(float& theta, const int& type) {
		if (Physics::GetProjectionAngleElevated(spawnLocationOffset, targetLocation, projectileSpeed, theta, type)) {
			return true;
		}
		return false;
	}
	virtual void SetVelocity(const int& projectionType) {
		float theta;
		if (CalculateAngleOfProjection(theta, projectionType)) {
			float speedX;
			float speedZ;
			float diagonalDistance;
			float xDisplacement = targetLocation.x - spawnLocationOffset.x;
			float diagonalSpeed = projectileSpeed * cos(theta);
			if (xDisplacement != 0) {
				diagonalDistance = sqrt(sq(targetLocation.x - spawnLocationOffset.x) + sq(targetLocation.z - spawnLocationOffset.z));
				speedX = ((targetLocation.x - spawnLocationOffset.x) * (diagonalSpeed)) / diagonalDistance;
				speedZ = ((targetLocation.z - spawnLocationOffset.z) * (diagonalSpeed)) / diagonalDistance;
			}
			else {
				speedX = 0.0f;
				speedZ = diagonalSpeed;
			}
			
			velocity = { speedX, projectileSpeed* sin(theta), speedZ };
		}
		else {
			velocity = { 0.0f, 10.0f, 0.0f };
			toDestroy = true;
		}
	}
protected:
	Vecf3 velocity;
	const Vecf3 targetLocation;
	const float projectileSpeed;
	bool toDestroy = false;
	const Vecf3 gravitationalAcc = { 0.0f, -9.81f, 0.0f };
};