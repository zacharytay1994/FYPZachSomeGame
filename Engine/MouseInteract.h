#pragma once

#include "Mouse.h"
#include "ZBuffer.h"
#include "Vec3.h"
#include "Graphics.h"
#include "EntityHandler.h"

#include <memory>

class MouseInteract {
public:
	MouseInteract(std::shared_ptr<ZBuffer>& zBuffer, Graphics& gfx)
		:
		zBuffer(zBuffer),
		gfx(gfx)
	{}
	Vecf2 TexPosAtMouse(Mouse& mouse) {
		if (mouse.GetPosX() > 0.0f && mouse.GetPosX() < gfx.ScreenWidth &&
			mouse.GetPosY() > 0.0f && mouse.GetPosY() < gfx.ScreenHeight)
		return zBuffer->pointBuffer[mouse.GetPosY() * gfx.ScreenWidth + mouse.GetPosX()];
	}
	Veci2 TexPosToGridPos(const int& gridSize, Vecf2& texPos) {
		return Veci2(int(gridSize * texPos.x), int(gridSize * texPos.y));
	}
	void SpawnEnemy(EntityHandler& eh, Mouse& mouse) {
		eh.AddEnemy(1.0f, TexPosToGridPos(eh.gridSize, TexPosAtMouse(mouse)));
	}
private:
	std::shared_ptr<ZBuffer> zBuffer;
	Graphics& gfx;
};