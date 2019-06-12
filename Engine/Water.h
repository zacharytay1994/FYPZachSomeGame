#pragma once

#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "SurfaceDirectionalLighting.h"
#include "PlaneVertex.h"
#include "Graphics.h"
#include "ZBuffer.h"
#include "MatTemplate.h"
#include "Distortion.h"

#include <memory>

class Water {
public:
	Water(Graphics& gfx, std::shared_ptr<ZBuffer>& zbuffer, const float& yOffset) 
		:
		waterPipeline(std::make_unique<Pipeline<SurfaceDirectionalLighting>>(gfx, zbuffer)),
		waterPlane(PlaneVertex::GetPlaneHorizontalSplit<SurfaceDirectionalLighting::Vertex>(80.0f, 1)),
		zBuffer(zbuffer),
		yOffset(yOffset)
	{
		waterPipeline->effect.pixelShader.BindTexture("blueimage.bmp");
		waterPipeline->isWater = true;
	}
	void Update(float dt) {
		distortion.Update(dt);
	}
	void Draw(const Matf4& worldTransform, const Matf4& viewMatrix, const Matf4& projectionMatrix, const Vecf3& cameraPosition) {
		// binds camera position in pipeline to calculate fresnel
		waterPipeline->CalculateWaterStuff(cameraPosition);
		std::vector<Vecf3> passInMap = distortion.DistortNormalMap(5);
		waterPipeline->BindNormalMap(passInMap,distortion.NMWidth,distortion.NMHeight);
		waterPipeline->CalculateCameraClipPosition();
		// distort with dudvmap
		std::vector<Color> reflectionOut;
		std::vector<Color> refractionOut;
		distortion.SampleDuDv(zBuffer->reflectionBuffer.get(), zBuffer->refractionBuffer.get(), zBuffer->width, zBuffer->height, 3.0f, reflectionOut, refractionOut);
		// binds reflection buffer from zBuffer as surface texture using screen space coordinates
		waterPipeline->effect.pixelShader.BindBuffer(reflectionOut,
			refractionOut, zBuffer->width, zBuffer->height);
		waterPipeline->effect.vertexShader.BindWorld(worldTransform);
		waterPipeline->effect.vertexShader.BindView(viewMatrix);
		waterPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		waterPipeline->Draw(waterPlane);
	}
	float GetYOffSet() {
		return yOffset;
	}
private:
	// distance away from y = 0
	float yOffset;
	std::shared_ptr<ZBuffer> zBuffer;
	std::unique_ptr<Pipeline<SurfaceDirectionalLighting>> waterPipeline;
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> waterPlane;
	Distortion distortion;
};