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
#include "ConsoleBox.h"
#include "MouseInteract.h"

#include <string>
#include <sstream>
#include <memory>
#include <chrono>
#include <ctime>

class DebugWorld : public Scene {
public:
	// size of the world in world space coordinates (worldSize by worldSize large)
	const float worldSize = 50.0f;
	// size of the world in terms of a grid (gridSize by gridSize large)
	const int gridSize = 100;
	// hence, worldSize/gridSize = density of vertices in world space
public:
	DebugWorld(Graphics& gfx, const std::shared_ptr<FontList> fontList)
		:
		sceneZBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		Scene("Debug world", sceneZBuffer, gfx),
		terrainWithPath(std::make_shared<TerrainWithPath>(gfx, sceneZBuffer, "heightmap2.bmp", "parchmentpaper.bmp", worldSize, gridSize, 0.0f, 8.0f)), // TerrainWithPath(graphics, zbuffer, heightmap, surface texture, world size, grid size, min world height, max world height)
		entityHandler(gfx, sceneZBuffer, worldSize, gridSize, terrainWithPath, consoleBox),
		consoleBox(std::make_shared<ConsoleBox>(gfx, sceneZBuffer, fontList))
	{
		//entityHandler.AddSolid(1.0f, { 55, 0, 45 });
		// let entityHandler know about the heightmap to implicitly place some entities
		entityHandler.SetHeightMap(terrainWithPath->GetHeightMap());
		//entityHandler.AddEnemy(1.0f, { 8.0f, 0.1f, 8.0f });
		entityHandler.AddEnemy(1.0f, { 25, 25 });
		//entityHandler.PopulateRandomTurrets(15);
		//entityHandler.AddTurret(2.5f, { 50, 50 });
		// make known to world terrain of solid obstacle entities
		terrainWithPath->SyncWithWorldEntities(entityHandler.solidBuffer);
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
		testingval = (testingval >= (worldSize - 0.5f))?0.0f:testingval + 1.0f * dt;
		//terrainWithPath.FindPath({ 0.0f, (worldSize/gridSize)/2.0f, (worldSize/2.0f - 0.2f) }, { -(worldSize/2.0f) + testingval, (worldSize / gridSize) / 2.0f, -(worldSize / 2.0f - 0.2f) });
		consoleBox->ChildUpdates(kbd, mouse, dt);
		// testing mouse interactivity
		if (kbd.KeyIsPressed('R')) {
			mouseAct = true;
		}
		if (mouse.LeftIsPressed() && mouseAct) {
			mouseInteract.SpawnEnemy(entityHandler, mouse);
			mouseAct = false;
		}
		//ss << test.x << "," << test.y << std::endl;
		OutputDebugString(ss.str().c_str());
	}
	virtual void Draw() override {
		// clearing shared zbuffer between all pipelines per frame
		sceneZBuffer->Clear();
		// scene world and camera transformation matrices
		const Matf4 projectionMatrix = Matf4::Projection(4.0f, 3.0f, 1.0f, 100.0f);
		camRotInverse = Matf4::Identity() * Matf4::RotationY(camY) * Matf4::RotationX(-0.8f);
		const Matf4 viewMatrix = Matf4::Translation(-cameraPosition) * camRotInverse;
		const Vecf3 translate = { 0.0f, 0.0f, 0.0f };
		const Matf4 worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		const Matf4 orientation = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y);
		// binding transformation matrices to external components and draws them
		// draws all entities in entity handler
		entityHandler.Draw(viewMatrix, projectionMatrix);
		// draws world terrain and path found TerrainWithPath::FindPath()
		terrainWithPath->Draw(worldTransform, viewMatrix, projectionMatrix);
		//entityHandler.DrawDebugDisplay();
		//terrainWithPath.DrawPath(viewMatrix, projectionMatrix);
		consoleBox->Draw(viewMatrix, projectionMatrix);
	}
//public:
//	// mouse interactivity
//	MouseInteract mouseInteract;

private:
	// shared zbuffer of scene
	std::shared_ptr<ZBuffer> sceneZBuffer;
	// orientation euler angles
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	// projection inverse matrices, directional light position, camera variables
	Matf4 camRotInverse = Matf4::Identity() * Matf4::RotationX(-0.8f) * Matf4::RotationY(camY);
	Vecf3 cameraPosition = { -6.0f, 14.0f, -8.0f };
	Vecf3 lightPosition = { 0.0f, 0.0f, 0.6f };
	const float cameraSpeed = 4.0f;
	float camY = 0.0f;
	// external components
	// entityHandler object, handles all scene objects that inherit the entity class
	EntityHandler entityHandler;
	// object that represents the scene terrain and pathing grid, handles pathfinding and terrain rendering
	std::shared_ptr<TerrainWithPath> terrainWithPath;
	// other testing variables
	float testingval = 0.0f;
	// world variables

	// environment object
	std::shared_ptr<ConsoleBox> consoleBox;

	// debugging stuff
	std::wstringstream ss;
	bool mouseAct = false;
};