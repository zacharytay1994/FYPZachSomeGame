#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "TexCube.h"
#include "SurfaceDirectionalLighting.h"


class Entity {
public:
	Entity(const float& size, const Vecf3& loc, const float& worldSize, const int& gridSize)
		:
		size(size),
		locationOnBoard2D({ (int)loc.x, (int)loc.z }),
		locationOnBoard3D(loc),
		worldSize(worldSize),
		gridSize(gridSize),
		cubeList(TexCube::GetPlain<Pipeline<SurfaceDirectionalLighting>::Vertex>(size))
	{}
	Entity(const float& size, const Veci2& loc, const float& worldSize, const int& gridSize)
		:
		Entity(size, { (float)loc.x, 0.0f, (float)loc.y }, worldSize, gridSize)
	{}
	// constructor mainly for turret entities who implicitly have their y coordinate set for them
	Entity(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize)
		:
		Entity(size, {(float)loc.x, heightDisplaced, (float)loc.y}, worldSize, gridSize)
	{}
	// constructor for basic projectiles
	Entity(const float& size, const Vecf3& loc)
		:
		size(size),
		cubeList(TexCube::GetPlain<Pipeline<SurfaceDirectionalLighting>::Vertex>(size)),
		spawnLocationOffset(loc)
	{}
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual Veci2 Get2DLocation() {
		return locationOnBoard2D;
	}
	virtual void Set2DLocation(const Veci2& loc) {
		locationOnBoard2D = loc;
	}
	virtual void Set3DLocation(const Vecf3& loc) {
		locationOnBoard3D = loc;
	}
	virtual IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> GetCubeList() {
		return cubeList;
	}
	virtual Vecf3 Calculate3DLocationOffset() {
		float yOffset = (size / 2) - cellRadius;
		float xOffset = (-(worldSize/2) + cellRadius) + locationOnBoard3D.x * cellDiameter;
		float zOffset = ((worldSize/2) - cellRadius) - locationOnBoard3D.z * cellDiameter;
		spawnLocationOffset = { xOffset, yOffset + locationOnBoard3D.y, zOffset };
		return spawnLocationOffset;
	}
	virtual void SetVelocity(const Vecf3& vel) {
		velocity = vel;
	}
	virtual Vecf3 GetSpawnLocationOffset() {
		return spawnLocationOffset;
	}
	virtual void CalculateBoundaries() {
		leftBound = spawnLocationOffset.x - size / 2;
		rightBound = spawnLocationOffset.x + size / 2;
		farBound = spawnLocationOffset.z + size / 2;
		nearBound = spawnLocationOffset.z - size / 2;
		upBound = spawnLocationOffset.y + size / 2;
		bottomBound = spawnLocationOffset.y - size / 2;
	}
	virtual bool PointCollide(const Vecf3& point) {
		if (point.x > leftBound && point.x < rightBound &&
			point.y > bottomBound && point.y < upBound &&
			point.z > nearBound && point.z < farBound) {
			return true;
		}
		return false;
	}
protected:
	// 2d location based on board range (x, z) (0-99, 0-99), y coordinate is always world coordinate not (0-99)
	Veci2 locationOnBoard2D; 
	Vecf3 locationOnBoard3D;
	float size;
	float worldSize;
	int gridSize;
	float cellDiameter = worldSize / gridSize;
	float cellRadius = cellDiameter / 2;
	// boundaries
	float leftBound = 0.0f;
	float rightBound = 0.0f;
	float farBound = 0.0f;
	float nearBound = 0.0f;
	float upBound = 0.0f;
	float bottomBound = 0.0f;
	// world coordinates
	Vecf3 spawnLocationOffset;
	// movement vector
	Vecf3 velocity = { 0.0f, 0.0f, 0.0f };
	// triangleList
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> cubeList;
};
