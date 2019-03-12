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
#include "Mat3.h"

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
	const float dt = 1.0f / 60.0f;
	if (wnd.kbd.KeyIsPressed('Q'))
	{
		theta_x += dTheta * dt;
	}
	if (wnd.kbd.KeyIsPressed('W'))
	{
		theta_y += dTheta * dt;
	}
	if (wnd.kbd.KeyIsPressed('E'))
	{
		theta_z += dTheta * dt;
	}
	if (wnd.kbd.KeyIsPressed('A'))
	{
		theta_x -= dTheta * dt;
	}
	if (wnd.kbd.KeyIsPressed('S'))
	{
		theta_y -= dTheta * dt;
	}
	if (wnd.kbd.KeyIsPressed('D'))
	{
		theta_z -= dTheta * dt;
	}
}

void Game::ComposeFrame()
{
	// Get cube lines
	IndexedLineList linelist = cube.GetLines();
	// Build rotation matrix
	const Matf3 rot =
		Matf3::RotationZ(theta_z) *
		Matf3::RotationX(theta_x) *
		Matf3::RotationY(theta_y);
	for (Vecf3& v : linelist.vertices) {
		// Rotate transform
		v *= rot;
		// Translate 1.0 in the +z axis
		v += {0.0f, 0.0f, 1.0f};
		trans.Transform(v);
	}
	for (std::_Vector_const_iterator i = linelist.indices.cbegin(), end = linelist.indices.cend(); i != end; std::advance(i, 2)) {
		gfx.DrawLine(linelist.vertices[*i], linelist.vertices[*std::next(i)], Colors::White);
	}

	// Draw Line
	/*Matf2 rot = Matf2::Rotation(theta_z);
	Vecf2 vec1({ 0.0f, 0.0f });
	Vecf2 vec2({ 0.0f, -0.5f });
	vec2 *= rot;
	trans.Transform(vec1);
	trans.Transform(vec2);
	gfx.DrawLine(vec1, vec2, Colors::White);*/
}
