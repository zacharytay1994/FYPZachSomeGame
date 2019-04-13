#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Rect.h"
#include "Surface.h"

#include <string>
#include <vector>

class MenuSceneParent {
public:
	MenuSceneParent(const std::string& name, const std::string& filename, Graphics& gfx)
		:
		name(name),
		menuTexture(std::make_unique<Surface>(filename)),
		gfx(gfx),
		screenWidth(gfx.ScreenWidth),
		screenHeight(gfx.ScreenHeight),
		surfaceWidth(menuTexture->GetWidth()),
		surfaceHeight(menuTexture->GetHeight()),
		screenAlphaW(float(surfaceWidth) / float(screenWidth)),
		screenAlphaH(float(surfaceHeight) / float(screenHeight))
	{}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw() {
		for (int x = 0; x < screenWidth; x++) {
			for (int y = 0; y < screenHeight; y++) {
				gfx.PutPixel(x, y, (Color)(menuTexture->GetPixel(int(x * screenAlphaW), int(y * screenAlphaH))));
			}
		}
	};
	virtual void OnClick() = 0;
	virtual ~MenuSceneParent() = default;
	const std::string& GetName() const {
		return name;
	}
public:
	const std::unique_ptr<Surface> menuTexture;
private:
	const std::string name;
	const std::vector<Rect> clickableRects;
	// draw screen variables
	Graphics& gfx;
	const int screenWidth;
	const int screenHeight;
	const int surfaceWidth;
	const int surfaceHeight;
	const float screenAlphaW;
	const float screenAlphaH;
};
