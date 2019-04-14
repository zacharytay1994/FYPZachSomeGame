#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Rect.h"
#include "Surface.h"

#include <string>
#include <stack>
#include <vector>

class MenuSceneParent {
public:
	MenuSceneParent(const std::string& name, const std::string& filename, Graphics& gfx, std::stack<std::unique_ptr<MenuSceneParent>>& menuScenes)
		:
		name(name),
		surface(Surface(filename)),
		gfx(gfx),
		screenWidth(gfx.ScreenWidth),
		screenHeight(gfx.ScreenHeight),
		surfaceWidth(surface.GetWidth()),
		surfaceHeight(surface.GetHeight()),
		screenAlphaW(float(surfaceWidth) / float(screenWidth)),
		screenAlphaH(float(surfaceHeight) / float(screenHeight)),
		menuScenes(menuScenes)
	{}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) = 0;
	virtual void AddDraw() = 0;
	virtual void Draw() {
		for (int x = 0; x < screenWidth; x++) {
			for (int y = 0; y < screenHeight; y++) {
				gfx.PutPixel(x, y, (Color)(surface.GetPixel(int(x * screenAlphaW), int(y * screenAlphaH))));
			}
		}
		AddDraw();
	}
	virtual bool OnClick(const Mouse::Event& e) {
		if (e.GetType() == Mouse::Event::Type::LPress) {
			return true;
		}
		return false;
	}
	// basic events open and close
	virtual void CloseDown() {
		menuScenes.pop();
	}
	virtual ~MenuSceneParent() = default;
	const std::string& GetName() const {
		return name;
	}
protected:
	std::vector<Rect> clickableRects;
	std::stack<std::unique_ptr<MenuSceneParent>>& menuScenes;
	// draw screen variables
	Graphics& gfx;
	Surface surface;
	const int screenWidth;
	const int screenHeight;
	const int surfaceWidth;
	const int surfaceHeight;
	const float screenAlphaW;
	const float screenAlphaH;
	const float perX = screenWidth / 100.0f;
	const float perY = screenHeight / 100.0f;
	int eventID = 0;
private:
	const std::string name;
};
