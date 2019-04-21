#pragma once

#include "Entity.h"
#include "EntityOne.h"

#include <vector>
#include <memory>

class EntityHandler {
public:
	EntityHandler() {}

	void Update() {

	}

	void Draw(const std::shared_ptr<Pipeline<SurfaceDirectionalLighting>>& pipeline) {
		std::vector<std::unique_ptr<Entity>>::iterator end = entityBuffer.end();
		for (std::vector<std::unique_ptr<Entity>>::iterator x = entityBuffer.begin(); x != end; std::advance(x, 1)) {
			translateVector = (*x)->CalculateLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			pipeline->effect.vertexShader.BindWorld(worldTransform);
			pipeline->Draw((*x)->GetCubeList());
		}
	}
	// add entity (size, location)
	void AddEntity(const float& size, const Veci2& loc) {
		entityBuffer.emplace_back(std::make_unique<EntityOne>(size, loc));
	}
private:
	std::vector<std::unique_ptr<Entity>> entityBuffer;
	Vecf3 translateVector;
	Matf4 worldTransform;
};