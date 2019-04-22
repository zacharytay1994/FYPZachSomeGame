#pragma once

#include "Entity.h"
#include "EntityOne.h"
#include "Graphics.h"
#include "MatTemplate.h"
#include "GridAStar.h"

#include <vector>
#include <memory>

class EntityHandler {
public:
	EntityHandler(Graphics& gfx) 
		:
		entityZBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		entityPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, entityZBuffer)),
		grid(gfx, planeSize)
	{
		entityPipeline->effect.pixelShader.BindTexture("greenimage.bmp");
	}

	void Update() {
		std::vector<std::unique_ptr<Entity>>::iterator end = entityBuffer.end();
		for (std::vector<std::unique_ptr<Entity>>::iterator x = entityBuffer.begin(); x != end; std::advance(x, 1)) {
			(*x)->Update();
		}
		end = solidBuffer.end();
		for (std::vector<std::unique_ptr<Entity>>::iterator x = solidBuffer.begin(); x != end; std::advance(x, 1)) {
			(*x)->Update();
		}
		InitGrid();
	}

	void InitGrid() {
		grid.UpdateWalkable(solidBuffer);
	}

	void Draw(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		entityPipeline->BeginFrame();
		entityPipeline->effect.vertexShader.BindView(viewMatrix);
		entityPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		
		// loop through normal entities
		std::vector<std::unique_ptr<Entity>>::iterator end = entityBuffer.end();
		for (std::vector<std::unique_ptr<Entity>>::iterator x = entityBuffer.begin(); x != end; std::advance(x, 1)) {
			translateVector = (*x)->Calculate3DLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
		}
		// loop through solid entities
		end = solidBuffer.end();
		for (std::vector<std::unique_ptr<Entity>>::iterator x = solidBuffer.begin(); x != end; std::advance(x, 1)) {
			translateVector = (*x)->Calculate3DLocationOffset();
			(*x)->CalculateBoundaries();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
		}
		grid.Draw(viewMatrix, projectionMatrix);
	}
	// add entity (size, location)
	void AddEntity(const float& size, const Veci2& loc) {
		entityBuffer.emplace_back(std::make_unique<EntityOne>(size, loc));
	}
	void AddEntity(const float& size, const Veci3& loc) {
		entityBuffer.emplace_back(std::make_unique<EntityOne>(size, loc));
	}
	void AddSolid(const float& size, const Veci2& loc) {
		solidBuffer.emplace_back(std::make_unique<EntityOne>(size, loc));
		(*(solidBuffer.end() - 1))->Calculate3DLocationOffset();
		(*(solidBuffer.end() - 1))->CalculateBoundaries();
	}
	void AddSolid(const float& size, const Veci3& loc) {
		solidBuffer.emplace_back(std::make_unique<EntityOne>(size, loc));
		(*(solidBuffer.end() - 1))->Calculate3DLocationOffset();
		(*(solidBuffer.end() - 1))->CalculateBoundaries();
	}
private:
	std::vector<std::unique_ptr<Entity>> entityBuffer;
	std::vector <std::unique_ptr<Entity>> solidBuffer;
	Vecf3 translateVector;
	Matf4 worldTransform;

	// pipeline stuff 
	std::shared_ptr<ZBuffer> entityZBuffer;
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> entityPipeline;

	// grid stuff
	const float planeSize = 10.0f;
	GridAStar grid;
};