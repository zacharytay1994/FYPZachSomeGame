#pragma once

#include "Scene.h"
#include "MatTemplate.h"
#include "SurfaceDirectionalLighting.h"
#include "Pipeline.h"
#include "ZBuffer.h"
#include "PlaneVertex.h"
#include "EntityHandler.h"
#include "Pathfinding.h"
#include "Terrain.h"
#include "TerrainWithPath.h"

#include <string>
#include <sstream>
#include <memory>
#include <chrono>
#include <ctime>

class DebugWorld : public Scene {
public:
	DebugWorld(Graphics& gfx)
		:
		Scene("Debug world"),
		sceneZBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		entityHandler(gfx, sceneZBuffer),
		terrainWithPath(gfx, sceneZBuffer, "heightmap2.bmp", "sandimage2.bmp", 20.0f, 100, 0.0f, 5.0f) // TerrainWithPath(graphics, zbuffer, heightmap, surface texture, world size, grid size, min world height, max world height)
	{
		entityHandler.AddSolid(1.0f, { 55, 0, 45 });
		entityHandler.AddSolid(2.0f, { 76, 0, 76 });
		entityHandler.AddSolid(1.0f, { 25, 0, 25 });
		entityHandler.AddSolid(0.5f, { 36, 0, 46 });
		entityHandler.AddSolid(1.5f, { 75, 0, 22 });
		entityHandler.AddSolid(1.0f, { 50, 0, 66 });
		entityHandler.AddSolid(1.5f, { 75, 0, 50 });
		entityHandler.AddSolid(1.2f, { 50, 0, 20 });
		entityHandler.AddSolid(1.0f, { 45, 0, 79 });
		entityHandler.AddSolid(0.5f, { 21, 0, 69 });
		entityHandler.AddSolid(1.5f, { 35, 0, 70 });
		entityHandler.AddSolid(1.0f, { 10, 0, 30 });
		entityHandler.AddSolid(1.5f, { 10, 0, 50 });
		// make known to world terrain of solid obstacle entities
		terrainWithPath.SyncWithWorldEntities(entityHandler.solidBuffer);
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		// camera movement
		if (kbd.KeyIsPressed('W')) {
			cameraPosition += Vecf4{ 0.0f, 0.0f, 1.0f, 0.0f } * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('A')) {
			cameraPosition += Vecf4{ -1.0f, 0.0f, 0.0f, 0.0f } * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('S')) {
			cameraPosition += Vecf4{ 0.0f, 0.0f, -1.0f, 0.0f } * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('D')) {
			cameraPosition += Vecf4{ 1.0f, 0.0f, 0.0f, 0.0f } * cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('N')) {
			cameraPosition += Vecf4{ 0.0f, 1.0f, 0.0f, 0.0f } *cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('M')) {
			cameraPosition += Vecf4{ 0.0f, -1.0f, 0.0f, 0.0f } *cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('J')) {
			camY += 1.0f * dt;
		}
		if (kbd.KeyIsPressed('K')) {
			camY -= 1.0f * dt;
		}
		// updating all entities in the entity buffer
		entityHandler.Update(kbd, mouse, dt);
		// updating world terrain, shifting worldEndPos per frame FindPath(worldStartPos, worldEndPos)
		testingval = (testingval >= 19.5f)?0.0f:testingval + 1.0f * dt;
		terrainWithPath.FindPath({ 0.0f, 0.5f, 9.8f }, { -5.0f + testingval, 0.5f, -9.8f });
	}
	virtual void Draw() override {
		// clearing shared zbuffer between all pipelines per frame
		sceneZBuffer->Clear();
		// scene world and camera transformation matrices
		const Matf4 projectionMatrix = Matf4::Projection(4.0f, 3.0f, 1.0f, 20.0f);
		camRotInverse = Matf4::Identity() * Matf4::RotationY(camY) * Matf4::RotationX(-0.8f);
		const Matf4 viewMatrix = Matf4::Translation(-cameraPosition) * camRotInverse;
		const Vecf3 translate = { 0.0f, 0.0f, 0.0f };
		const Matf4 worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		const Matf4 orientation = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y);
		// binding transformation matrices to external components and draws them
		// draws all entities in entity handler
		entityHandler.Draw(viewMatrix, projectionMatrix);
		// draws world terrain and path found TerrainWithPath::FindPath()
		terrainWithPath.Draw(worldTransform, viewMatrix, projectionMatrix);
	}
private:
	// shared zbuffer of scene
	std::shared_ptr<ZBuffer> sceneZBuffer;
	// orientation euler angles
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	// projection inverse matrices, directional light position, camera variables
	Matf4 camRotInverse = Matf4::Identity() * Matf4::RotationX(-0.8f) * Matf4::RotationY(camY);
	Vecf3 cameraPosition = { 0.0f, 5.0f, -5.0f };
	Vecf3 lightPosition = { 0.0f, 0.0f, 0.6f };
	const float cameraSpeed = 4.0f;
	float camY = 0.0f;
	// external components
	// entityHandler object, handles all scene objects that inherit the entity class
	EntityHandler entityHandler;
	// object that represents the scene terrain and pathing grid, handles pathfinding and terrain rendering
	TerrainWithPath terrainWithPath;
	// other testing variables
	float testingval = 0.0f;
};