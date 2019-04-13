#pragma once

#include "MenuSceneParent.h"

class StartMenuScene : public MenuSceneParent {
public:
	StartMenuScene(Graphics& gfx) 
		:
		MenuSceneParent("Start Menu", "startmenutest1.bmp", gfx)
	{
		const float perX = screenWidth / 100.0f;
		const float perY = screenHeight / 100.0f;
		// first button
		clickableRects.emplace_back(Rect(Vecf2(50.0f * perX,50.0f * perY), 30.0f * perX, 20.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(50.0f * perX, 75.0f * perY), 30.0f * perX, 20.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(87.0f * perX, 18.0f * perY), 14.0f * perX, 24.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(86.0f * perX, 65.7f * perY), 27.0f * perX, 3.6f * perY));
		clickableRects.emplace_back(Rect(Vecf2(17.7f * perX, 38.8f * perY), 4.1f * perX, 7.4f * perY));
		clickableRects.emplace_back(Rect(Vecf2(7.8f * perX, 77.7f * perY), 3.1f * perX, 29.6f * perY));
	}
	void DrawSurface(const Surface& surface) {
		for (int i = 0; i < screenWidth; i++) {
			for (int j = 0; j < screenHeight; j++) {
				gfx.PutPixel(i, j, (Color)(surface.GetPixel(int(i * screenAlphaW), int(j * screenAlphaH))));
			}
		}
	}
	virtual void SetSurface() {
		if (clickableRects[0].IsOverlap(mousePosition)) {
			surface = Surface(supSurfaces[1]);
		}
		else if (clickableRects[1].IsOverlap(mousePosition)) {
			surface = Surface(supSurfaces[2]);
		}
		else if (clickableRects[2].IsOverlap(mousePosition)) {
			surface = Surface(supSurfaces[3]);
		}
		else if (clickableRects[3].IsOverlap(mousePosition)) {
			surface = Surface(supSurfaces[4]);
		}
		else if (clickableRects[4].IsOverlap(mousePosition)) {
			surface = Surface(supSurfaces[5]);
		}
		else if (clickableRects[5].IsOverlap(mousePosition)) {
			surface = Surface(supSurfaces[6]);
		}
		else {
			surface = Surface(supSurfaces[0]);
		}
	}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		while (!mouse.IsEmpty()) {
			const auto e = mouse.Read();
			mousePosition = { (float)e.GetPosX(), (float)e.GetPosY() };
		}
	}
	virtual void OnClick() override {

	}
private:
	Surface supSurfaces[7] = { Surface("startmenutest1.bmp"), Surface("startmenutest2.bmp"), Surface("startmenutest3.bmp"),
	Surface("startmenutest4.bmp"), Surface("startmenutest5.bmp"), Surface("startmenutest6.bmp"), Surface("startmenutest7.bmp") };
	Vecf2 mousePosition;
};
