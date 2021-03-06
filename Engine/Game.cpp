/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "TestScene.h"
#include "DebugWorld.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	fontList(std::make_shared<FontList>())
{
	scenes.push_back(std::make_unique<DebugWorld>(gfx, fontList));
	curScene = scenes.begin();

	// push in start menu
	menuScenes.push(std::make_unique<LevelOneGUI>(gfx, menuScenes, scenes, fontList));
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	const float dt = 1.0f / 60.0f;
	(*curScene)->Update(wnd.kbd, wnd.mouse, dt);

	// update top menu scene
	if (!menuScenes.empty()) {
		menuScenes.top()->Update(wnd.kbd, wnd.mouse, dt);
	}
}

void Game::ComposeFrame()
{
	(*curScene)->Draw();

	// draw top menu scene
	if (!menuScenes.empty()) {
		menuScenes.top()->Draw();
	}
}
