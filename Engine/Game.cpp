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
#include "Mat2.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	cube( 0.5f )
{
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
}

void Game::ComposeFrame()
{
	// Get cube lines
	//IndexedLineList linelist = cube.GetLines();
	//for (Vecf3& v : linelist.vertices) {
	//	// translate 1.0 in the +z axis
	//	v += {0.0f, 0.0f, 1.0f};
	//	trans.Transform(v);
	//}
	//for (std::_Vector_const_iterator i = linelist.indices.cbegin(), end = linelist.indices.cend(); i != end; std::advance(i, 2)) {
	//	gfx.DrawLine(linelist.vertices[*i], linelist.vertices[*std::next(i)], Colors::White);
	//}
	Vecf2 vec1({ 0.0f, 0.0f });
	Vecf2 vec2({ 0.3f, 0.5f });
	trans.Transform(vec1);
	trans.Transform(vec2);
	gfx.DrawLine(vec1, vec2, Colors::White);
}
