#pragma once

#include "Scene.h"
#include "PlaneVertex.h"
#include "MatTemplate.h"
#include "SurfaceDirectionalLighting.h"
#include "TextureEffectPerPixel.h"
#include "Pipeline.h"
#include "ZBuffer.h"
#include "Board.h"
#include "Font.h"
#include "Console.h"

#include <assert.h>

class ConsoleScene : public Scene {
public:
	ConsoleScene(Graphics& gfx, const std::shared_ptr<FontList> fontList)
		:
		Scene("Console Scene"),
		zBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		consolePipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, zBuffer)),
		planeList(PlaneVertex::GetPlane<Pipeline<SurfaceDirectionalLighting>::Vertex>(3.0f)),
		surface(Surface("parchmentpaper.bmp")),
		fontList(fontList)
	{
		consolePipeline->effect.pixelShader.BindSurface(surface);
		consolePipeline->effect.geomShader.BindDirectionalLight(Vecf3(0.0f, 0.0f, -0.6f));
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
		console.Update(kbd);
		//theta_x += 1.0f * dt;
		demoVal += 0.02;
		theta_y += std::cos(demoVal) * dt/2;
		//consolePipeline->effect.vertexShader.SetTime(dt);
	}
	virtual void Draw() override {
		consolePipeline->BeginFrame();
		// transformation matrices
		const Matf4 projectionMatrix = Matf4::Projection(4.0f, 3.0f, 1.0f, 50.0f);
		const Matf4 viewMatrix = Matf4::Translation(-cameraPosition) * camRotInverse;
		const Vecf3 translate = { 0.0f, 0.0f, 3.0f };
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
	// font writing variables
	Surface surface; // surface to alter
	const std::shared_ptr<FontList>& fontList;
	Console<SurfaceDirectionalLighting> console = Console(fontList->CourierNew30by54, consolePipeline, surface);
	float demoVal = 0;
};