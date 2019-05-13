#pragma once

#include "Pipeline.h"
#include "IndexedLineList.h"
#include "SurfaceDirectionalLighting.h"
#include "Graphics.h"
#include "ZBuffer.h"
#include "TexCube.h"

class SkyBox {
public:
	SkyBox(Graphics& gfx, std::shared_ptr<ZBuffer>& zBuffer) 
		:
		zBuffer(zBuffer),
		skyBoxPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, zBuffer)),
		skyCubeList(TexCube::GetSkyCube<Pipeline<SurfaceDirectionalLighting>::Vertex>(40.0f))
	{
		skyBoxPipeline->effect.pixelShader.BindTexture("test.bmp");
		skyBoxPipeline->effect.geomShader.BindAmbientLight(1.0f);
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0 {}
	void Update(Keyboard&kbd, Mouse& mouse, float dt) {
		ChildUpdates(kbd, mouse, dt);
	}
	void Draw(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		skyBoxPipeline->effect.vertexShader.BindView(viewMatrix);
		skyBoxPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		skyBoxPipeline->Draw(skyCubeList);
	}
protected:
	// pipeline stuff
	std::shared_ptr<ZBuffer> zBuffer;
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> skyBoxPipeline;
	// indexed triangle list
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> skyCubeList;
};