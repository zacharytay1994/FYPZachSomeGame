#pragma once

#include "MenuSceneParent.h"
#include "Graphics.h"

class StartMenuScene : public MenuSceneParent {
public:
	StartMenuScene(Graphics& gfx) 
		:
		MenuSceneParent("Start Menu", "pikachumeme.bmp", gfx)
	{}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {

	}
	virtual void OnClick() override {

	}
private:
	
};
