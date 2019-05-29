#pragma once

#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "SurfaceDirectionalLighting.h"
#include "PlaneVertex.h"
#include "Graphics.h"
#include "ZBuffer.h"
#include "MatTemplate.h"

#include <memory>

class Water {
public:
	Water(Graphics& gfx, std::shared_ptr<ZBuffer>& zbuffer, const float& yOffset, Vecf3& cameraPosition, float& camX, float& camY) 
		:
		waterPipeline(std::make_unique<Pipeline<SurfaceDirectionalLighting>>(gfx, zbuffer)),
		waterPlane(PlaneVertex::GetPlaneHorizontalSplit<SurfaceDirectionalLighting::Vertex>(50.0f, 10)),
		yOffset(yOffset),
		zBuffer(zbuffer),
		cameraPosition(cameraPosition),
		cameraXRotation(camX),
		cameraYRotation(camY)
	{
		waterPipeline->effect.pixelShader.BindTexture("blueimage.bmp");
		waterPipeline->isWater = true;
	}
	void Draw(const Matf4& worldTransform, const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		// get reflection
		/*Vecf3 reflectedCameraPosition = -cameraPosition;
		reflectedCameraPosition.y = reflectedCameraPosition.y * -1.0f;
		Matf4 camRotInverse = Matf4::Identity() * Matf4::RotationY(cameraYRotation) * Matf4::RotationX(-cameraXRotation);
		const Matf4 reflectedViewMatrix = Matf4::Translation(reflectedCameraPosition) * camRotInverse;
		waterPipeline->toDraw = false;
		waterPipeline->effect.vertexShader.BindView(reflectedViewMatrix);*/
		// draw water
		waterPipeline->effect.pixelShader.BindBuffer(zBuffer->reflectionBuffer.get(), zBuffer->refractionBuffer.get(),
			zBuffer->width, zBuffer->height);
		waterPipeline->effect.vertexShader.BindWorld(worldTransform);
		waterPipeline->effect.vertexShader.BindView(viewMatrix);
		waterPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		waterPipeline->Draw(waterPlane);
	}
	float GetYOffSet() {
		return yOffset;
	}
	void GetReflectionBuffer() {
		
	}
private:
	float yOffset;
	std::shared_ptr<ZBuffer> zBuffer;
	std::unique_ptr<Pipeline<SurfaceDirectionalLighting>> waterPipeline;
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> waterPlane;

	// surface buffers
	std::vector<Color> reflectionBuffer;
	Vecf3& cameraPosition;
	float& cameraXRotation;
	float& cameraYRotation;
};