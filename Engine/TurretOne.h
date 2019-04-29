#pragma once

#include "TurretParent.h"

class TurretOne : public TurretParent {
public:
	TurretOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize)
		:
		TurretParent(size, loc, heightDisplaced, worldSize, gridSize)
	{}
	virtual void Update() override {

	}
	virtual void Draw() override {

	}
private:
};