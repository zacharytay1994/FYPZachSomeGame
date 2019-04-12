#pragma once

#include "Scene.h"
#include "TexCube.h"
#include "MatTemplate.h"
#include "TextureEffectPerPixel.h"
#include "Pipeline.h"
#include "ZBuffer.h"

class TestScene : public Scene {
public:
	TestScene( Graphics& gfx) 
		:
		Scene("Test Scene"),
		zBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		pipelinePerPixel(gfx, zBuffer),
		testList(TexCube::GetWrap<Pipeline<TextureEffectPerPixel>::Vertex>(1.0f))
	{
		pipelinePerPixel.effect.pixelShader.BindTexture("whiteimage.bmp");
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		// camera movement
		if (kbd.KeyIsPressed('W')) {
			cameraPosition += Vecf4{ 0.0f, 0.0f, 1.0f, 0.0f } *cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('A')) {
			cameraPosition += Vecf4{ -1.0f, 0.0f, 0.0f, 0.0f } *cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('S')) {
			cameraPosition += Vecf4{ 1.0f, 0.0f, -1.0f, 0.0f } *cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('D')) {
			cameraPosition += Vecf4{ 1.0f, 0.0f, 0.0f, 0.0f } *cameraSpeed * dt;
		}
	}
	virtual void Draw() override {
		pipelinePerPixel.BeginFrame();
		// transformation matrices
		const Matf4 projectionMatrix = Matf4::Projection(2.0f, 2.0f, 1.0f, 10.0f);
		const Matf4 viewMatrix = Matf4::Translation(-cameraPosition) * camRotInverse;
		const Vecf3 translate = {0.0f, 0.0f, 2.5f};
		const Matf4 worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		// binding transformation matrices to pipeline
		pipelinePerPixel.effect.vertexShader.BindWorld(worldTransform);
		pipelinePerPixel.effect.vertexShader.BindView(viewMatrix);
		pipelinePerPixel.effect.vertexShader.BindProjection(projectionMatrix);
		// setting other pipeline values
		pipelinePerPixel.effect.vertexShader.SetTime(time);
		// draw pipeline
		pipelinePerPixel.Draw(testList);
	}

private:
	// pipeline stuff
	std::shared_ptr<ZBuffer> zBuffer;
	Pipeline<TextureEffectPerPixel> pipelinePerPixel;
	float time = 0.0f;
	// indexed triangle list
	IndexedTriangleList<Pipeline<TextureEffectPerPixel>::Vertex> testList;
	// orientation euler angles
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	// projection inverse matrices and camera stuff
	Matf4 camRotInverse = Matf4::Identity();
	Vecf3 cameraPosition = { 0.0f, 0.0f, 0.0f };
	Vecf3 modelPosition = { 0.0f, 0.0f, 2.5f };
	Vecf3 lightPosition = { 0.0f, 0.0f, 0.6f };
	const float cameraSpeed = 1.0f;
};