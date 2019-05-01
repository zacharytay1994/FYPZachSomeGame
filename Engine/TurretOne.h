#pragma once

#include "TurretParent.h"
#include "ProjectileOne.h"
#include "Physics.h"

#include <sstream>
#include <string>

class TurretOne : public TurretParent {
public:
	TurretOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize)
		:
		TurretParent(size, loc, heightDisplaced, worldSize, gridSize)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		if (kbd.KeyIsPressed('P')) {
			LoadProjectileWithTarget(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(2.0f, 0.0f, 0.0f));
		}
		if (kbd.KeyIsPressed('O')) {
			LoadProjectileWithTarget(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(4.0f, 0.0f, 0.0f));
		}
		if (kbd.KeyIsPressed('I')) {
			LoadProjectileWithTarget(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(8.0f, 0.0f, 0.0f));
		}
		if (kbd.KeyIsPressed('U')) {
			LoadProjectileWithTarget(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(10.0f, 0.0f, 0.0f));
		}
		if (kbd.KeyIsPressed('Y')) {
			LoadProjectileWithTarget(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(12.0f, 0.0f, 0.0f));
		}
		if (kbd.KeyIsPressed('T')) {
			/*float cosTheta;
			float sinTheta;
			Physics::GetProjectionAngle(spawnLocationOffset, spawnLocationOffset + Vecf3(2.0f, 0.0f, 0.0f), 5.0f, cosTheta, sinTheta);
			ss << cosTheta << " , " << sinTheta << std::endl;
			OutputDebugString(ss.str().c_str());*/
		}
	}
	virtual void Draw() override {

	}
	void LoadProjectileWithTarget(ProjectileType p, const Vecf3& targetPos) {
		ProjectileHolder.push_back(static_cast<int>(p));
		float cosTheta;
		float sinTheta;
		if (Physics::GetProjectionAngle(spawnLocationOffset, targetPos, 12.0f, cosTheta, sinTheta)) {
			VelocityHolder.emplace_back(Vecf3(12.0f*std::cos(cosTheta), 12.0f*std::sin(sinTheta), 0.0f));
			ss << cosTheta << " , " << sinTheta << std::endl;
			OutputDebugString(ss.str().c_str());
		}
		else {
			VelocityHolder.emplace_back(Vecf3(0.0f, 10.0f, 0.0f));
		}
		
	}
private:
	std::wstringstream ss;
};