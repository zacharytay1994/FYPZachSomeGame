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
	bool GetSceneScreenPositon(Mouse& mouse, Veci2& parse) {
		float sceneDisplayXOffset = (float)gfx.ScreenWidthDisplay / 3.0f;
		float sceneDisplayYOffset = 0.0f;
		float sceneDisplayWidth = ((float)gfx.ScreenWidthDisplay / 3.0f)*2.0f;
		float sceneDisplayHeight = ((float)gfx.ScreenHeightDisplay / 3.0f)*2.0f;
		if (mouse.GetPosX() > (gfx.ScreenWidthDisplay/3) && mouse.GetPosX() < gfx.ScreenWidthDisplay &&
			mouse.GetPosY() > 0 && mouse.GetPosY() < sceneDisplayHeight) {
			parse.x = int((((float)mouse.GetPosX()-sceneDisplayXOffset) / sceneDisplayWidth)*gfx.ScreenWidth);
			parse.y = int((((float)mouse.GetPosY()-sceneDisplayYOffset) / sceneDisplayHeight)*gfx.ScreenHeight);
			return true;
		}
		return false;
	}
	Vecf2 TexPosAtMouse(Mouse& mouse) {
		Veci2 parseIn = { 0, 0 };
		GetSceneScreenPositon(mouse, parseIn);
		std::wstringstream ss;
		ss << parseIn.x << " , " << parseIn.y << std::endl;
		//OutputDebugString(ss.str().c_str());
		return zBuffer->pointBuffer[parseIn.y * gfx.ScreenWidth + parseIn.x];
		/*if (mouse.GetPosX() > 0.0f && mouse.GetPosX() < gfx.ScreenWidthDisplay &&
			mouse.GetPosY() > 0.0f && mouse.GetPosY() < gfx.ScreenHeightDisplay) {
			return zBuffer->pointBuffer[mouse.GetPosY() * gfx.ScreenWidth + mouse.GetPosX()];
		}
		return { 0.5f, 0.5f };*/
	}
	Veci2 TexPosToGridPos(const int& gridSize, Vecf2& texPos) {
		gridPos = Veci2(int(gridSize * texPos.x), int(gridSize * texPos.y));
		return gridPos;
	}
	void SpawnEnemy(EntityHandler& eh, Mouse& mouse) {
		switch (entitySelected) {
		case 0:
			eh.MoveArrow(TexPosToGridPos(eh.gridSize, TexPosAtMouse(mouse)));
			break;
		case 1:
			eh.AddTurret(1.0f, TexPosToGridPos(eh.gridSize, TexPosAtMouse(mouse)));
			break;
		case 2:
			eh.AddEnemy(0.5f, TexPosToGridPos(eh.gridSize, TexPosAtMouse(mouse)));
			break;
		case 3:
			eh.AddBuilding(3.0f, TexPosToGridPos(eh.gridSize, TexPosAtMouse(mouse)));
			break;
		}
	}
	void SetEntitySelected(const int& entity) {
		entitySelected = entity;
	}
	Veci2 GetGridPos() {
		return gridPos;
	}
private:
	std::shared_ptr<ZBuffer>& zBuffer;
	Graphics& gfx;
	// entity selected menu variables
	int entitySelected = 0;
	Veci2 gridPos;
};