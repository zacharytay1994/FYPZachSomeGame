#pragma once

#include "Scene.h"
#include "MatTemplate.h"
#include "SurfaceDirectionalLighting.h"
#include "Pipeline.h"
#include "ZBuffer.h"
#include "PlaneVertex.h"
#include "EntityHandler.h"

#include <string>
#include <sstream>
#include <memory>

class DebugWorld : public Scene {
public:
	DebugWorld(Graphics& gfx)
		:
		Scene("Debug world"),
		groundZBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		entityZBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		groundPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, groundZBuffer)),
		entityPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, entityZBuffer)),
		planeList(PlaneVertex::GetPlaneHorizontal<Pipeline<SurfaceDirectionalLighting>::Vertex>(10.0f))
	{
		groundPipeline->effect.pixelShader.BindTexture("whiteimage.bmp");
		entityPipeline->effect.pixelShader.BindTexture("greenimage.bmp");
		entityHandler.AddEntity(1.0f, { 25, 0, 25 });
		entityHandler.AddEntity(2.0f, { 50, 50 });
		entityHandler.AddEntity(0.3f, { 75, 25, 75 });
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		// camera movement
		if (kbd.KeyIsPressed('W')) {
			cameraPosition += Vecf4{ 0.0f, 0.0f, 1.0f, 0.0f } /**!camRotInverse*/ * cameraSpeed * dt;
			OutputDebugStringW(L"test");
			OutputDebugStringW(L"\n");
		}
		if (kbd.KeyIsPressed('A')) {
			cameraPosition += Vecf4{ -1.0f, 0.0f, 0.0f, 0.0f }/* *!camRotInverse*/ * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('S')) {
			cameraPosition += Vecf4{ 0.0f, 0.0f, -1.0f, 0.0f } /**!camRotInverse*/ * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('D')) {
			cameraPosition += Vecf4{ 1.0f, 0.0f, 0.0f, 0.0f } /**!camRotInverse*/ * cameraSpeed * dt;
		}
		//theta_x += 1.0f * dt;
		//theta_y = 0.2f * 60;
		//std::wstringstream ss;
		//ss << mouse.GetPosX() << ',' << mouse.GetPosY() << std::endl;
		//OutputDebugString(ss.str().c_str());

		//if (zBuffer->pointBuffer[mouse.GetPosY() * zBuffer->width + mouse.GetPosX()] != Vecf2(0.0f, 0.0f)) {
		//	std::wstringstream ss2;
		//	//ss2 << zBuffer->pointBuffer[mouse.GetPosY() * zBuffer->width + mouse.GetPosX()].x << ',' << zBuffer->pointBuffer[mouse.GetPosY() * zBuffer->width + mouse.GetPosX()].y << "test" << std::endl;
		//	float xVal = zBuffer->pointBuffer[mouse.GetPosY() * zBuffer->width + mouse.GetPosX()].x;
		//	float yVal = zBuffer->pointBuffer[mouse.GetPosY() * zBuffer->width + mouse.GetPosX()].y;
		//	float zVal = zBuffer->At(mouse.GetPosX(), mouse.GetPosY())/* - cameraPosition.z*/;
		//	/*xVal = ((xVal / 960.0f) - 1.0f) * zVal;
		//	yVal = ((-yVal / 540.0f) - 1.0f) * zVal;*/
		//	/*xVal = xVal * zVal;
		//	yVal = yVal * zVal;*/
		//	ss2 << xVal << ',' << yVal << ',' << zVal << std::endl;
		//	OutputDebugString(ss2.str().c_str());
		//}
		entityHandler.Update();
	}
	virtual void Draw() override {
		groundPipeline->BeginFrame();
		entityPipeline->BeginFrame();
		// transformation matrices
		const Matf4 projectionMatrix = Matf4::Projection(4.0f, 3.0f, 1.0f, 20.0f);
		const Matf4 viewMatrix = Matf4::Translation(-cameraPosition) * camRotInverse;
		const Vecf3 translate = { 0.0f, 0.0f, 0.0f };
		const Matf4 worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		const Matf4 orientation = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y);
		// binding transformation matrices to pipelines
		groundPipeline->effect.vertexShader.BindWorld(worldTransform);
		groundPipeline->effect.vertexShader.BindView(viewMatrix);
		groundPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		// draw board/world
		groundPipeline->Draw(planeList);

		// bind entity view transforms and draw all entities
		entityPipeline->effect.vertexShader.BindView(viewMatrix);
		entityPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		entityHandler.Draw(entityPipeline);
		// setting other pipeline values

		
	}
private:
	// pipeline stuff
	std::shared_ptr<ZBuffer> groundZBuffer;
	std::shared_ptr<ZBuffer> entityZBuffer;
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> groundPipeline;
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> entityPipeline;
	float time = 0.0f;
	// indexed triangle list
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> planeList;
	// orientation euler angles
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	// projection inverse matrices and camera stuff
	Matf4 camRotInverse = Matf4::Identity() * Matf4::RotationX(-0.8f);
	Vecf3 cameraPosition = { 0.0f, 5.0f, 0.0f };
	Vecf3 modelPosition = { 0.0f, 0.0f, 2.5f };
	Vecf3 lightPosition = { 0.0f, 0.0f, 0.6f };
	const float cameraSpeed = 4.0f;
	// world entities
	EntityHandler entityHandler;
};