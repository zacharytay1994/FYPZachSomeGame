#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "TexCube.h"


class Entity {
public:
	Entity(const float& size, const Veci3& loc)
		:
		size(size),
		locationOnBoard2D({ loc.x, loc.z }),
		locationOnBoard3D(loc),
		cubeList(TexCube::GetPlain<Pipeline<SurfaceDirectionalLighting>::Vertex>(size))
	{}
	Entity(const float& size, const Veci2& loc) 
		:
		Entity(size, {loc.x, 0, loc.y})
	{}
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual Veci2 Get2DLocation() {
		return locationOnBoard2D;
	}
	virtual void Set2DLocation(const Veci2& loc) {
		locationOnBoard2D = loc;
	}
	virtual void Set3DLocation(const Veci3& loc) {
		locationOnBoard3D = loc;
	}
	virtual IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> GetCubeList() {
		return cubeList;
	}
	virtual Vecf3 Calculate3DLocationOffset() {
		float yOffset = size / 2 + locationOnBoard3D.y * 0.1f;
		float xOffset = (-5.0f + 0.05f) + locationOnBoard3D.x * 0.1f;
		float zOffset = (5.0f - 0.05f) - locationOnBoard3D.z * 0.1f;
		spawnLocationOffset = { xOffset, yOffset, zOffset };
		return spawnLocationOffset;
	}
	virtual void SetVelocity(const Vecf3& vel) {
		velocity = vel;
	}
protected:
	// 2d location based on board range (x, z) (0-99, 0-99)
	Veci2 locationOnBoard2D; 
	Veci3 locationOnBoard3D;
	float size;
	// location offset in world space
	Vecf3 spawnLocationOffset;
	// movement vector
	Vecf3 velocity = { 0.0f, 0.0f, 0.0f };
	// triangleList
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> cubeList;
};
