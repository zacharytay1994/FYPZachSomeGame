#pragma once

#include "ProjectileParent.h"

#include <sstream>
#include <string>

class ProjectileOne : public ProjectileParent{
public:
	ProjectileOne(const Vecf3& loc, Vecf3& vel) 
		:
		ProjectileParent(0.5f, loc), // (size, location)
		velocity(vel)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		/*spawnLocationOffset += initialHVel * dt;
		spawnLocationOffset += initialVVel * dt;
		initialVVel += gravitationalAcc * dt;*/
		spawnLocationOffset += velocity * dt;
		velocity += gravitationalAcc * dt;
		check += velocity * dt;
		/*if (check.x > 2 && spawnLocationOffset.y > 0.0f) {
			ss << "reached 2" << std::endl;
			OutputDebugString(ss.str().c_str());
		}*/
	}
	virtual void Draw() override {

	}
private:
	/*Vecf3 initialVVel = { 0.0f, 10.0f, 0.0f };
	Vecf3 initialHVel = { 5.0f, 0.0f, 0.0f };
	Vecf3 testVel = { 5.0f * std::cos(2.458f), 5.0f * std::sin(2.458f), 0.0f };*/
	Vecf3 velocity;
	const Vecf3 gravitationalAcc = { 0.0f, -9.81f, 0.0f };
	std::wstringstream ss;
	Vecf3 check = { 0.0f,0.0f, 0.0f };
};