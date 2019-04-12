#pragma once

#include "Scene.h"
#include "TexCube.h"
#include "MatTemplate.h"
#include "TextureEffectPerPixel.h"
#include "Pipeline.h"
#include "ZBuffer.h"

class TestScene : public Scene {
public:
private:
	// pipeline stuff
	std::shared_ptr<ZBuffer> zBuffer;
	Pipeline<TextureEffectPerPixel> pipelinePerPixel;
};