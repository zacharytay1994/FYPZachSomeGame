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
		terrainWithPath(gfx, sceneZBuffer, "heightmap2.bmp", "whiteimage.bmp", 20.0f, 100, 0.0f, 5.0f), // TerrainWithPath(graphics, zbuffer, heightmap, surface texture, world size, grid size, min world height, max world height)
		entityHandler(gfx, sceneZBuffer, 20.0f, 100)
	{
		//entityHandler.AddSolid(1.0f, { 55, 0, 45 });
		
		// make known to world terrain of solid obstacle entities
		entityHandler.SetHeightMap(terrainWithPath.GetHeightMap());
		entityHandler.AddTurret(0.5f, { 45, 79 });
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
		/*testingval = (testingval >= 19.5f)?0.0f:testingval + 1.0f * dt;
		terrainWithPath.FindPath({ 0.0f, 0.5f, 9.8f }, { -5.0f + testingval, 0.5f, -9.8f });*/
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
		//terrainWithPath.DrawPath(viewMatrix, projectionMatrix);
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
	// world variables
	const float worldSize = 20.0f;
	const int gridSize = 100;
};