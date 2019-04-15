#pragma once

#include "Scene.h"
#include "PlaneVertex.h"
#include "MatTemplate.h"
#include "SurfaceDirectionalLighting.h"
#include "Pipeline.h"
#include "ZBuffer.h"
#include "Board.h"

class ConsoleScene : public Scene {
public:
	ConsoleScene(Graphics& gfx)
		:
		Scene("Console Scene"),
		zBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		consolePipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, zBuffer)),
		planeList(PlaneVertex::GetPlane<Pipeline<SurfaceDirectionalLighting>::Vertex>(2.0f))
	{
		consolePipeline->effect.pixelShader.BindTexture("whiteimage.bmp");
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		// camera movement
		if (kbd.KeyIsPressed('W')) {
			//cameraPosition += Vecf4{ 0.0f, 0.0f, 1.0f, 0.0f } *!camRotInverse * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('A')) {
			//cameraPosition += Vecf4{ -1.0f, 0.0f, 0.0f, 0.0f } *!camRotInverse * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('S')) {
			//cameraPosition += Vecf4{ 0.0f, 0.0f, -1.0f, 0.0f } *!camRotInverse * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('D')) {
			//cameraPosition += Vecf4{ 1.0f, 0.0f, 0.0f, 0.0f } *!camRotInverse * cameraSpeed * dt;
		}
		//theta_x += 1.0f * dt;
		theta_y += 0.5f * dt;
	}
	virtual void Draw() override {
		consolePipeline->BeginFrame();
		// transformation matrices
		const Matf4 projectionMatrix = Matf4::Projection(4.0f, 3.0f, 1.0f, 50.0f);
		const Matf4 viewMatrix = Matf4::Translation(-cameraPosition) * camRotInverse;
		const Vecf3 translate = { 0.0f, 0.0f, 2.5f };
		const Matf4 worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		const Matf4 orientation = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y);
		// binding transformation matrices to pipeline
		consolePipeline->effect.vertexShader.BindWorld(worldTransform);
		consolePipeline->effect.vertexShader.BindView(viewMatrix);
		consolePipeline->effect.vertexShader.BindProjection(projectionMatrix);
		// setting other pipeline values

		consolePipeline->Draw(planeList);
	}
private:
	// pipeline stuff
	std::shared_ptr<ZBuffer> zBuffer;
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> consolePipeline;
	float time = 0.0f;
	// indexed triangle list
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> planeList;
	// orientation euler angles
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	// projection inverse matrices and camera stuff
	Matf4 camRotInverse = Matf4::Identity() * Matf4::RotationX(0.1f);
	Vecf3 cameraPosition = { 0.0f, 0.0f, 0.0f };
	Vecf3 modelPosition = { 0.0f, 0.0f, 2.5f };
	Vecf3 lightPosition = { 0.0f, 0.0f, 0.6f };
	const float cameraSpeed = 4.0f;
};