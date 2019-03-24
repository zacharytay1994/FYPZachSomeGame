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
#include "Pipeline.h"
#include "Tessellate.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	cube( 1.0f ),
	zBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
	pipeline(gfx, zBuffer),
	pipelineLight(gfx, zBuffer),
	testList(TexCube::GetWrap<Pipeline<TextureEffect>::Vertex>(1.0f)),
	lightList(TexCube::GetWrap<Pipeline<TextureEffect>::Vertex>(0.1f)),
	tessellateList(Tessallate::GetTessellate<Pipeline<TextureEffect>::Vertex>(testList))
{
	// bind texture to pipeline
	pipeline.effect.pixelShader.BindTexture("sandimage.bmp");
	for (int i = 0; i < 4; i++) {
		tessellateList = Tessallate::GetTessellate<Pipeline<TextureEffect>::Vertex>(tessellateList);
	}
	pipelineLight.effect.pixelShader.BindTexture("whiteimage.bmp");
	pipelineLight.effect.vertexShader.SetTransValues(0.0f, 0.0f, 0.0f);

	pipelineLight.effect.geomShader.SetIsLight(true);
	// set colors to vertices in colorList
	/*colorList.vertices[0].color = (Vecf3)Colors::Red;
	colorList.vertices[1].color = (Vecf3)Colors::Blue;
	colorList.vertices[2].color = (Vecf3)Colors::Cyan;
	colorList.vertices[3].color = (Vecf3)Colors::Gray;
	colorList.vertices[4].color = (Vecf3)Colors::Green;
	colorList.vertices[5].color = (Vecf3)Colors::Magenta;
	colorList.vertices[6].color = (Vecf3)Colors::Black;
	colorList.vertices[7].color = (Vecf3)Colors::White;*/

	/*for (int i = 0; i < surf.GetHeight(); i++) {
		for (int j = 0; j < surf.GetWidth(); j++) {
			surf.PutPixel(j, i, Colors::Red);
		}
	}*/
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
	if (wnd.kbd.KeyIsPressed('O')) {
		zVal += 1.0 * dt;
	}
	if (wnd.kbd.KeyIsPressed('P')) {
		zVal -= 1.0 * dt;
	}
	if (wnd.kbd.KeyIsPressed('H')) {
		lightPosX -= 0.5f * dt;
	}
	if (wnd.kbd.KeyIsPressed('K')) {
		lightPosX += 0.5f * dt;
	}
	if (wnd.kbd.KeyIsPressed('U')) {
		lightPosY += 0.5f * dt;
	}
	if (wnd.kbd.KeyIsPressed('J')) {
		lightPosY -= 0.5f * dt;
	}
	if (wnd.kbd.KeyIsPressed('Y')) {
		lightPosZ += 0.5f * dt;
	}
	if (wnd.kbd.KeyIsPressed('I')) {
		lightPosZ -= 0.5f * dt;
	}
	time += dt;
}

void Game::ComposeFrame()
{
	pipeline.BeginFrame();
	//cPipeline.BeginFrame();
	// bind transforms
	const Matf3 rot =
		Matf3::RotationZ(theta_z) *
		Matf3::RotationX(theta_x) *
		Matf3::RotationY(theta_y);
	pipeline.effect.vertexShader.BindRotation(rot);
	const Vecf3 translate = { 0.0f, 0.0f, zVal };
	pipeline.effect.vertexShader.BindTranslation(translate);
	pipeline.effect.vertexShader.SetTime(time);
	pipeline.effect.geomShader.BindTime(time);
	pipeline.effect.geomShader.SetLightPosition({ lightPosX, lightPosY, lightPosZ });

	// draw light source
	pipelineLight.effect.vertexShader.BindRotation(rot);
	pipelineLight.effect.vertexShader.BindTranslation({ lightPosX, lightPosY, lightPosZ });
	
	//cPipeline.BindRotation(rot);

	//cPipeline.BindTranslation({ 0.5f, 0.5f, 3.0f });
	/*cPipeline.Draw(colorList);

	cPipeline.BindTranslation(translate * 2);
	cPipeline.Draw(colorList);*/
	// draw textured cube pipeline
	pipeline.Draw(tessellateList);
	pipelineLight.Draw(lightList);

	/*pipeline.effect.vertexShader.BindTranslation({ 0.5f, 0.5f, 3.0f });
	pipeline.Draw(testList);*/

	//--------------------------------------------------------------------------------------------------

	//// Get tex cube line
	//IndexedTriangleList triangleList = texCube.getTriangleList();
	//const Matf3 rot =
	//	Matf3::RotationZ(theta_z) *
	//	Matf3::RotationX(theta_x) *
	//	Matf3::RotationY(theta_y);
	//// Model space to world space
	//for (TextureVertex& v : triangleList.texVertices) {
	//	// Rotate transform
	//	v.pos *= rot;
	//	// Translate 1.0 in the +z axis
	//	v.pos += Vecf3{0.0f, 0.0f, zVal};
	//}
	//// Cull triangles
	//for (size_t i = 0, end = triangleList.indices.size() / 3; i < end; i++) {
	//	const Vecf3& v0 = triangleList.texVertices[triangleList.indices[i * 3]].pos;
	//	const Vecf3& v1 = triangleList.texVertices[triangleList.indices[i * 3 + 1]].pos;
	//	const Vecf3& v2 = triangleList.texVertices[triangleList.indices[i * 3 + 2]].pos;
	//	//float test2 = v1 * v0;
	//	float test = (v1 - v0) % (v2 - v0) * v0;
	//	triangleList.cullFlags[i] = test >= 0.0f;
	//}
	//// World space to screen space
	//for (TextureVertex& v : triangleList.texVertices) {
	//	trans.Transform(v.pos);
	//}
	//for (size_t i = 0, end = triangleList.indices.size() / 3; i < end; i++) {
	//	if (!triangleList.cullFlags[i]) {
	//		gfx.DrawTriangleTex(
	//			triangleList.texVertices[triangleList.indices[i * 3]],
	//			triangleList.texVertices[triangleList.indices[i * 3 + 1]],
	//			triangleList.texVertices[triangleList.indices[i * 3 + 2]],
	//			texSurf
	//		);
	//	}
	//}
	//--------------------------------------------------------------------------------------------------

	/*gfx.DrawTriangleTex(tvec1, tvec2, tvec3, texSurf);
	gfx.DrawTriangleTex(tvec1, tvec21, tvec31, texSurf);
	gfx.DrawSprite(300, 300, texSurf);*/
	
	//--------------------------------------------------------------------------------------------------

	//// Array of colors
	//const Color colors[12] = {
	//	Colors::White, Colors::Blue, Colors::Cyan, Colors::Gray, Colors::Green, Colors::Magenta, Colors::LightGray, 
	//	Colors::Red, Colors::Yellow, Colors::White, Colors::Blue, Colors::Cyan
	//};
	//// Get cube lines
	//IndexedTriangleList trianglelist = cube.GetTriangles();
	//IndexedLineList linelist = cube.GetLines();
	//// Build rotation matrix
	//const Matf3 rot =
	//	Matf3::RotationZ(theta_z) *
	//	Matf3::RotationX(theta_x) *
	//	Matf3::RotationY(theta_y);
	//// Model space to world space
	//for (Vecf3& v : trianglelist.vertices) {
	//	// Rotate transform
	//	v *= rot;
	//	// Translate 1.0 in the +z axis
	//	v += {0.0f, 0.0f, zVal};
	//}
	//// Cull triangles
	//for (size_t i = 0, end = trianglelist.indices.size() / 3; i < end; i++) {
	//	const Vecf3& v0 = trianglelist.vertices[trianglelist.indices[i * 3]];
	//	const Vecf3& v1 = trianglelist.vertices[trianglelist.indices[i * 3 + 1]];
	//	const Vecf3& v2 = trianglelist.vertices[trianglelist.indices[i * 3 + 2]];
	//	float test2 = v1 * v0;
	//	float test = (v1 - v0) % (v2 - v0) * v0;
	//	trianglelist.cullFlags[i] = test >= 0.0f;
	//}
	//// World space to screen space
	//for (Vecf3& v : trianglelist.vertices) {
	//	trans.Transform(v);
	//}
	//for (size_t i = 0, end = trianglelist.indices.size() / 3; i < end; i++) {
	//	if (!trianglelist.cullFlags[i]) {
	//		gfx.DrawTriangle(trianglelist.vertices[trianglelist.indices[i * 3]], trianglelist.vertices[trianglelist.indices[i * 3 + 1]],
	//			trianglelist.vertices[trianglelist.indices[i * 3 + 2]], colors[i]);
	//	}
	//}

	//--------------------------------------------------------------------------------------------------

	/*gfx.DrawSprite(100, 100, surf);
	gfx.DrawSprite(200, 200, texSurf);*/

	//--------------------------------------------------------------------------------------------------

	//for (std::_Vector_const_iterator i = trianglelist.indices.cbegin(), end = trianglelist.indices.cend(); i != end; std::advance(i, 3)) {
	//	gfx.DrawTriangle(trianglelist.vertices[*i], trianglelist.vertices[*std::next(i)], trianglelist.vertices[*std::next(i, 2)], 
	//		colors[std::distance(trianglelist.indices.cbegin(),i)/3]);
	//	//gfx.DrawLine(linelist.vertices[*i], linelist.vertices[*std::next(i)], Colors::White);
	//}

	//--------------------------------------------------------------------------------------------------

	// Draw Line
	/*Matf2 rot = Matf2::Rotation(theta_z);
	Vecf2 vec1({ 0.0f, 0.0f });
	Vecf2 vec2({ 0.0f, -0.5f });
	vec2 *= rot;
	trans.Transform(vec1);
	trans.Transform(vec2);
	gfx.DrawLine(vec1, vec2, Colors::White);*/
}
