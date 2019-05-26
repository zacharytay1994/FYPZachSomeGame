#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "TexCube.h"
#include "SurfaceDirectionalLighting.h"
#include "ZBuffer.h"
#include "Graphics.h"
#include "MessageDispatcher.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Clock.h"

#include <cmath>
#include <queue>
#include <ctime>

class EntityQueryHandler;
class Entity {
public:
	struct DebugMessage {
		bool operator()(const DebugMessage& lhs, const DebugMessage& rhs) const {
			return lhs > rhs;
		}
		bool operator>(const DebugMessage& rhs) const {
			return id > rhs.id;
		}
		bool operator<(const DebugMessage& rhs) const {
			return timeElapsed < rhs.timeElapsed;
		}
		bool operator==(const DebugMessage& rhs) const {
			return timeElapsed == rhs.timeElapsed;
		}
		bool operator>=(const DebugMessage& rhs) const {
			return timeElapsed >= rhs.timeElapsed;
		}
		bool operator<=(const DebugMessage& rhs) const {
			return timeElapsed <= rhs.timeElapsed;
		}
		std::string message;
		double timeElapsed;
		int id;
	};
	Entity(const float& size, const Vecf3& loc, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		size(size),
		locationOnBoard2D({ (int)loc.x, (int)loc.z }),
		locationOnBoard3D(loc),
		worldSize(worldSize),
		gridSize(gridSize),
		cubeList(TexCube::GetPlain<Pipeline<SurfaceDirectionalLighting>::Vertex>(size)),
		entityQueryHandler(entityQueryHandler)
	{
		SetUniqueID();
	}
	Entity(const float& size, const Veci2& loc, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		Entity(size, { (float)loc.x, 0.0f, (float)loc.y }, worldSize, gridSize, entityQueryHandler)
	{}
	// constructor mainly for turret entities who implicitly have their y coordinate set for them
	Entity(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		Entity(size, {(float)loc.x, heightDisplaced, (float)loc.y}, worldSize, gridSize, entityQueryHandler)
	{}
	// constructor for basic projectiles
	Entity(const float& size, const Vecf3& loc, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		size(size),
		cubeList(TexCube::GetPlain<Pipeline<SurfaceDirectionalLighting>::Vertex>(size)),
		spawnLocationOffset(loc),
		entityQueryHandler(entityQueryHandler)
	{
		SetUniqueID();
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void ChildMessage(const MessageDispatcher::Telegram& msg) = 0;
	// redundant draw call, not used as drawing is done by entityHandler and not by entity, left here for now
	virtual void Draw(const Matf4& viewMatrix, const Matf4& projectionMatrix) {}
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
	virtual void SetSpawnLocationOffsetY(const float& posIn) {
		spawnLocationOffset.y = posIn;
	}
	virtual void SetSpawnLocationOffsetYControlled(const float& posIn) {
		if (!(spawnLocationOffset.y > (posIn - 0.02f) && spawnLocationOffset.y < (posIn + 0.02))) {
			spawnLocationOffset.y += (posIn - spawnLocationOffset.y) / 2.0f;
		}
		else {
			spawnLocationOffset.y = posIn;
		}
	}
	virtual void SetSpawnLocationOffset(const Vecf3& posIn) {
		spawnLocationOffset = posIn;
	}
	virtual float GetSize() {
		return size;
	}
	virtual void CalculateBoundaries() {
		leftBound = spawnLocationOffset.x - size / 2 - cellDiameter * 2;
		rightBound = spawnLocationOffset.x + size / 2 + cellDiameter * 2;
		farBound = spawnLocationOffset.z + size / 2 + cellDiameter * 2;
		nearBound = spawnLocationOffset.z - size / 2 - cellDiameter * 2;
		upBound = spawnLocationOffset.y + size / 2 + cellDiameter * 2;
		bottomBound = spawnLocationOffset.y - size / 2 - cellDiameter * 2;
	}
	virtual bool PointCollide(const Vecf3& point) {
		if (point.x > leftBound && point.x < rightBound &&
			point.y > bottomBound && point.y < upBound &&
			point.z > nearBound && point.z < farBound) {
			return true;
		}
		return false;
	}
	// sets and increments
	void SetUniqueID() {
		entityUniqueID = nextValidID;
		nextValidID++;
	}
	int GetUniqueID() {
		return entityUniqueID;
	}
	// created Entity::DebugMessage and inserts it into debugQueue to be read by entityHandler to display to console
	void InsertDebugString(const std::string& string) {
		std::clock_t now = clock();
		double timeElapsed = now - Clock::begin;
		debugQueue.push_back({ string, timeElapsed, nextValidMessageID});
		nextValidMessageID++;
	}
	// handles message sent by other entities, executing based on msg.enumMessage from MessageDispatcher::Messages
	bool HandleMessage(const MessageDispatcher::Telegram& msg) {
		InsertDebugString("entity /y" + std::to_string(entityUniqueID) + " message received.");
		receivedMessage = true;
		ChildMessage(msg);
		return true;
	}
	// returns distance squared
	float DistanceBetween(const Vecf3 p1, const Vecf3 p2) {
		return sqrt((sq(p2.x - p1.x) + sq(p2.y - p1.y) + sq(p2.z - p1.z)));
	}
	Vecf2 CalculateGridPosition(const Vecf3& vecIn) {
		return Vecf2((vecIn.x + worldSize / 2.0f)/worldSize*(float)gridSize, gridSize - ((vecIn.z + worldSize / 2.0f)/worldSize*gridSize));
	}
	float GetWorldSize() {
		return worldSize;
	}
	int GetGridSize() {
		return gridSize;
	}
	float GetCellDiameter() {
		return cellDiameter;
	}
	bool WithinWorld() {
		return (spawnLocationOffset.x > -worldSize/2.0f && spawnLocationOffset.x < worldSize/2.0f && spawnLocationOffset.z > -worldSize/2.0f && spawnLocationOffset.z < worldSize/2.0f);
	}
public:
	std::vector<DebugMessage> debugQueue;

	// handles the query of other entities in the world
	std::shared_ptr<EntityQueryHandler> entityQueryHandler;

	// flag to see if entity still exists as simulated object in game world
	bool exists = true;
protected:
	// 2d location based on board range (x, z) (0-99, 0-99), y coordinate is always world coordinate not (0-99)
	Veci2 locationOnBoard2D; 
	Vecf3 locationOnBoard3D;
	float size;
	float worldSize;
	int	gridSize;
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
	Vecf3 spawnLocationOffset = {0.0f, 0.0f, 0.0f};
	Veci2 gridCellLocation;
	// movement vector
	Vecf3 velocity = { 0.0f, 0.0f, 0.0f };
	// triangleList
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> cubeList;

	// finite state machine variables
	int entityUniqueID;
	// next valid ID based on all entities in the world
	static int nextValidID;
	static int nextValidMessageID;
	// testing bool
	bool receivedMessage = false;
};
