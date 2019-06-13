#pragma once

#include "MenuSceneParent.h"
#include "FontList.h"
#include "SurfaceDirectionalLighting.h"
#include "Console.h"

class LevelOneGUI : public MenuSceneParent {
public:
	LevelOneGUI(Graphics& gfx, std::stack<std::unique_ptr<MenuSceneParent>>& stackIn, std::vector<std::unique_ptr<Scene>>& scenes,
		const std::shared_ptr<FontList>& fontList)
		:
		MenuSceneParent("Level One GUI", "levelguitwo.bmp", gfx, stackIn, scenes, Vecf3(255.0f, 255.0f, 255.0f), fontList),
		consoleGameLoopSurface("gameloopbackground2.bmp"),
		consoleDisplaySurface("consoledisplaybackground.bmp"),
		consoleWriteSurface("consolewritebackground.bmp"),
		consoleGameLoop(fontList->LevelGUIGameLoop, consoleGameLoopSurface),
		consoleDisplay(fontList->LevelGUIDisplay, consoleDisplaySurface),
		consoleWrite(fontList->LevelGUIWrite, consoleWriteSurface)
	{
		consoleGameLoop.isGUI = true;
		consoleDisplay.isGUI = true;
		consoleWrite.isGUI = true;
		consoleGameLoop.Write("turretone.attack(1,2,3)");
		consoleGameLoop.Write("this is to test if the display loops back around");
		consoleDisplay.Write("/yhi and welcome to level one.");
		consoleWrite.Write("something long");
		//clickableRects.emplace_back(Rect(Vecf2(12.5f * perX, 85.0f * perY), 18.5f * perX, 10.0f * perY));
	}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		while (!mouse.IsEmpty()) {
			const auto e = mouse.Read();
			mousePosition = { (float)e.GetPosX(), (float)e.GetPosY() };

			// set surface to be drawn
			/*if (clickableRects[0].IsOverlap(mousePosition)) {
				surface = supSurfaces[1];
				eventID = 1;
			}
			else {
				surface = supSurfaces[0];
				eventID = 0;
			}*/

			// on click event
			if (OnClick(e)) {
			}
		}
		if (kbd.KeyIsPressed('T')) {
			(*(scenes.begin()))->ReceiveMessage("yes");
			//scenes.pop_back();
			/*consoleGameLoop.Write("turretone.attack(1,2,3)");
			consoleDisplay.Write("turretone.attack(1,2,3)");*/
		}
		consoleScene.Update(kbd, mouse, dt);
		//consoleDisplay.Write("something");
	}
	virtual void AddDraw() override {
		/*if (consoleOut) {
			consoleScene.Draw();
		}*/
		consoleGameLoop.DrawSurfaceGUI(consoleOneStartX, consoleOneStartY, consoleOneWidth, consoleOneHeight, gfx);
		consoleDisplay.DrawSurfaceGUI(consoleTwoStartX, consoleTwoStartY, consoleTwoWidth, consoleTwoHeight, gfx);
		consoleWrite.DrawSurfaceGUI(consoleThreeStartX, consoleThreeStartY, consoleThreeWidth, consoleThreeHeight, gfx);
	}
private:
	// supplementary surface container
	const Surface supSurfaces[1] = { Surface("levelguione.bmp") };
	Vecf2 mousePosition;
	ConsoleScene consoleScene = ConsoleScene(gfx, fontList);
	bool consoleOut = false;
	// font writing variables
	Surface consoleGameLoopSurface; // surface to alter
	Surface consoleDisplaySurface;
	Surface consoleWriteSurface;
	Console<SurfaceDirectionalLighting> consoleGameLoop;
	Console<SurfaceDirectionalLighting> consoleDisplay;
	Console<SurfaceDirectionalLighting> consoleWrite;
	int guiOneMargin = 0;
	int guiTwoMargin = 0;
	int guiThreeMargin = 0;
	// console stuff
	// consoleGameLoop Variables ------------------------------
	int consoleOneStartX = 0 + guiOneMargin;
	int consoleOneStartY = int((((float)gfx.ScreenHeightDisplay/3.0f)*2.0f)/3.0f) + guiOneMargin;
	int consoleOneWidth = gfx.ScreenWidthDisplay/3 - guiOneMargin*2;
	int consoleOneHeight = int(((((float)gfx.ScreenHeightDisplay/3.0f)*2.0f)/3.0f)*2.0f) - guiOneMargin *2;
	// consoleDisplay Variables ----------------------------------
	int consoleTwoStartX = gfx.ScreenWidthDisplay/3 + guiTwoMargin;
	int consoleTwoStartY = (int)(((float)gfx.ScreenHeightDisplay/3.0f)*2.0f) + guiTwoMargin;
	int consoleTwoWidth = gfx.ScreenWidthDisplay/3 - guiTwoMargin * 2;
	int consoleTwoHeight = int(((float)gfx.ScreenHeightDisplay/3.0f) - (((float)gfx.ScreenHeightDisplay/3.0f)/6.0f)) - guiTwoMargin*2;
	// consoleWrite Variables ----------------------------------
	int consoleThreeStartX = gfx.ScreenWidthDisplay / 3 + guiThreeMargin;
	int consoleThreeStartY = gfx.ScreenHeightDisplay - int(((float)gfx.ScreenHeightDisplay / 3.0f) / 6.0f) + guiThreeMargin;
	int consoleThreeWidth = gfx.ScreenWidthDisplay / 3 - guiThreeMargin * 2;
	int consoleThreeHeight = int(((float)gfx.ScreenHeightDisplay / 3.0f) / 6.0f) - guiThreeMargin * 2;
};