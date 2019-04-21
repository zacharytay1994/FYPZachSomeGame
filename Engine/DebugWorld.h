#pragma once

#include "Scene.h"
#include "MatTemplate.h"
#include "SurfaceDirectionalLighting.h"
#include "Pipeline.h"
#include "ZBuffer.h"
#include "PlaneVertex.h"
#include "EntityHandler.h"
#include "GridAStar.h"

#include <string>
#include <sstream>
#include <memory>

class DebugWorld : public Scene {
public:
	DebugWorld(Graphics& gfx)
		:
		Scene("Debug world"),
		groundZBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		groundPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, groundZBuffer)),
		planeList(PlaneVertex::GetPlaneHorizontal<Pipeline<SurfaceDirectionalLighting>::Vertex>(planeSize)),
		entityHandler(gfx),
		grid(gfx, planeSize)
	{
		groundPipeline->effect.pixelShader.BindTexture("whiteimage.bmp");
		
		entityHandler.AddEntity(1.0f, { 25, 0, 25 });
		entityHandler.AddSolid(1.0f, { 50, 0, 50 });
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		// camera movement
		if (kbd.KeyIsPressed('W')) {
			cameraPosition += Vecf4{ 0.0f, 0.0f, 1.0f, 0.0f } * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('A')) {
			cameraPosition += Vecf4{ -1.0f, 0.0f, 0.0f, 0.0f } * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('S')) {
			cameraPosition += Vecf4{ 0.0f, 0.0f, -1.0f, 0.0f } * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('D')) {
			cameraPosition += Vecf4{ 1.0f, 0.0f, 0.0f, 0.0f } * cameraSpeed * dt;
		}
		entityHandler.Update();
	}
	virtual void Draw() override {
		groundPipeline->BeginFrame();
		
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
		// bind and draw external components
		grid.Draw(viewMatrix, projectionMatrix);
		entityHandler.Draw(viewMatrix, projectionMatrix);
	}
private:
	// pipeline stuff
	std::shared_ptr<ZBuffer> groundZBuffer;
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> groundPipeline;
	float time = 0.0f;
	// indexed triangle list
	const float planeSize = 10.0f;
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
	GridAStar grid;
};