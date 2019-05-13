#pragma once

#include "SkyBox.h"
#include "Console.h"
#include "FontList.h"

class ConsoleBox : public SkyBox {
public:
	ConsoleBox(Graphics& gfx, std::shared_ptr<ZBuffer>& zBuffer, const std::shared_ptr<FontList> fontList)
		:
		SkyBox(gfx, zBuffer),
		surface(Surface("parchmentpaper.bmp")),
		fontList(fontList)
	{}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {
		if (kbd.KeyIsPressed('I')) {
			console.Write("test");
		}
		if (kbd.KeyIsPressed('O')) {
			console.Write("this is test2");
		}
	}
private:
	// font writing variables
	Surface surface; // surface to alter
	const std::shared_ptr<FontList>& fontList;
	Console<SurfaceDirectionalLighting> console = Console(fontList->CourierNew30by54, skyBoxPipeline, surface);
	int counter = 1;
};