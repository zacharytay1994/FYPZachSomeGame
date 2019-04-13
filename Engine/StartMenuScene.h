#pragma once

#include "MenuSceneParent.h"
#include "Graphics.h"

class StartMenuScene : public MenuSceneParent {
public:
	StartMenuScene(Graphics& gfx) 
		:
		MenuSceneParent("Start Menu", "pikachumeme.bmp"),
		gfx(gfx),
		screenWidth(gfx.ScreenWidth),
		screenHeight(gfx.ScreenHeight),
		surfaceWidth(menuTexture->GetWidth()),
		surfaceHeight(menuTexture->GetHeight()),
		screenAlphaW(float(surfaceWidth) / float(screenWidth)),
		screenAlphaH(float(surfaceHeight) / float(screenHeight))
	{}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {

	}
	virtual void Draw() override {
		for (int x = 0; x < screenWidth; x++) {
			for (int y = 0; y < screenHeight; y++) {
				gfx.PutPixel(x, y, (Color)(menuTexture->GetPixel(int(x * screenAlphaW), int(y * screenAlphaH))));
			}
		}
	}
	virtual void OnClick() override {

	}
private:
	Graphics& gfx;
	const int screenWidth;
	const int screenHeight;
	const int surfaceWidth;
	const int surfaceHeight;
	const float screenAlphaW;
	const float screenAlphaH;
};
