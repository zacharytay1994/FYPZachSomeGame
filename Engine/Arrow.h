#pragma once

#include "Entity.h"
#include "SurfaceDirectionalLighting.h"
#include "TexCube.h"

class Arrow : public Entity {
public:
	Arrow(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		Entity(size, loc, heightDisplaced + 0.1f, worldSize, gridSize, entityQueryHandler),
		arrowList(TexCube::GetArrow<Pipeline<SurfaceDirectionalLighting>::Vertex>(size))
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		yaw += turnRate * dt;
		Bob(dt);
	}
	virtual void ChildMessage(const MessageDispatcher::Telegram& msg) override {}
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> GetArrowList() {
		return arrowList;
	}
	void Bob(const float& dt) {
		if (arrowYOffset < 1.0f && arrowYOffset > 0.0f) {
			if (!down) {
				arrowYOffset += bobRate * dt;
			}
			else if (down) {
				arrowYOffset -= bobRate * dt;
			}
		}
		else {
			if (!down) {
				down = true;
				arrowYOffset -= bobRate * 2 * dt;
			}
			else if (down) {
				down = false;
				arrowYOffset += bobRate * 2 * dt;
			}
		}
	}
public:
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> arrowList;
	float turnRate = 5.0f;
	float yaw = 0.0f;
	float arrowYOffset = 0.0f;
	float bobRate = 2.0f;
	bool down = true;
};