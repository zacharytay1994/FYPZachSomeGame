#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "TexCube.h"


class Entity {
public:
	Entity(const float& size, const Veci2& loc)
		:
		size(size),
		locationOnBoard(loc),
		cubeList(TexCube::GetPlain<Pipeline<SurfaceDirectionalLighting>::Vertex>(size))
	{}
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual Veci2 GetLocation() {
		return locationOnBoard;
	}
	virtual void SetLocation(const Veci2& loc) {
		locationOnBoard = loc;
	}
	virtual IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> GetCubeList() {
		return cubeList;
	}
	virtual Vecf3 CalculateLocationOffset() {
		float yOffset = size / 2;
		float xOffset = (-5.0f + 0.1f) + locationOnBoard.x * 0.1f;
		float zOffset = (5.0f - 0.1f) - locationOnBoard.y * 0.1f;
		spawnLocationOffset = { xOffset, yOffset, zOffset };
		return spawnLocationOffset;
	}
	virtual void SetVelocity(const Vecf3& vel) {
		velocity = vel;
	}
private:
	// 2d location based on board range (x, z) (0-99, 0-99)
	Veci2 locationOnBoard; 
	float size;
	// location offset in world space
	Vecf3 spawnLocationOffset;
	// movement vector
	Vecf3 velocity = { 0.0f, 0.0f, 0.0f };
	// triangleList
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> cubeList;
};
