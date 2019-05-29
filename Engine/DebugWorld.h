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
#include "Water.h"

#include <string>
#include <sstream>
#include <memory>
#include <chrono>
#include <ctime>

class DebugWorld : public Scene {
public:
	// size of the world in world space coordinates (worldSize by worldSize large)
	const float worldSize = 25.0f;
	// size of the world in terms of a grid (gridSize by gridSize large)
	const int gridSize = 100;
	// hence, worldSize/gridSize = density of vertices in world space
public:
	DebugWorld(Graphics& gfx, const std::shared_ptr<FontList> fontList)
		:
		sceneZBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		Scene("Debug world", sceneZBuffer, gfx),
		terrainWithPath(std::make_shared<TerrainWithPath>(gfx, sceneZBuffer, "heightmap1.bmp", "test.bmp", worldSize, gridSize, -15.0f, 5.0f)), // TerrainWithPath(graphics, zbuffer, heightmap, surface texture, world size, grid size, min world height, max world height)
		entityHandler(gfx, sceneZBuffer, worldSize, gridSize, terrainWithPath, consoleBox),
		consoleBox(std::make_shared<ConsoleBox>(gfx, sceneZBuffer, fontList)),
		water(std::make_shared<Water>(gfx, sceneZBuffer,0.0f, cameraPosition, camX, camY)),
		gfx(gfx)
	{
		//entityHandler.AddSolid(1.0f, { 55, 0, 45 });
		// let entityHandler know about the heightmap to implicitly place some entities
		entityHandler.SetHeightMap(terrainWithPath->GetHeightMap());
		//entityHandler.AddEnemy(1.0f, { 8.0f, 0.1f, 8.0f });
		/*entityHandler.AddBuilding(3.0f, { 60, 60 });
		entityHandler.AddBuilding(3.0f, { 30, 30 });
		entityHandler.AddBuilding(3.0f, { 70, 30 });
		entityHandler.AddBuilding(3.0f, { 30, 60 });
		entityHandler.AddBuilding(3.0f, { 50, 50 });
		entityHandler.AddBuilding(3.0f, { 55, 60 });*/
		
		//entityHandler.AddEnemy(1.0f, { 75, 75 });
		//entityHandler.AddEnemyAA(0.5f, { 75, 25 });
		//entityHandler.PopulateRandomTurrets(15);
		//entityHandler.AddTurret(2.5f, { 50, 50 });
		// make known to world terrain of solid obstacle entities
		terrainWithPath->SyncWithWorldEntities(entityHandler.buildingBuffer);
		/*Pipeline<SurfaceDirectionalLighting>::uprightVector = { 0.0f, 1.0f, 0.0f };
		Pipeline<SurfaceDirectionalLighting>::pitch = 0.0f;
		Pipeline<SurfaceDirectionalLighting>::roll = 0.0f;
		Pipeline<SurfaceDirectionalLighting>::yaw = 0.0f;*/
		//Pipeline<SurfaceDirectionalLighting>::pitch = 3.14f/4;
		//float test = Pipeline<SurfaceDirectionalLighting>::pitch;
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		clock++;
		// camera movement
		if (kbd.KeyIsPressed('W')) {
			zOffset += cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('A')) {
			xOffset -= cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('S')) {
			zOffset -= cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('D')) {
			xOffset += cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('N')) {
			yOffset += cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('M')) {
			yOffset -= cameraSpeed * dt;
		}
		if (kbd.KeyIsPressed('J')) {
			camY += 1.0f * dt;
			//Pipeline<SurfaceDirectionalLighting>::yaw = -camY;
		}
		if (kbd.KeyIsPressed('K')) {
			camY -= 1.0f * dt;
			//Pipeline<SurfaceDirectionalLighting>::yaw = -camY;
		}
		if (kbd.KeyIsPressed('U')) {
			camX += 1.0f * dt;
			//Pipeline<SurfaceDirectionalLighting>::pitch = -camY;
		}
		if (kbd.KeyIsPressed('H')) {
			camX -= 1.0f * dt;
			//Pipeline<SurfaceDirectionalLighting>::pitch = -camY;
		}
		if (kbd.KeyIsPressed('L')) {
			camZ += 1.0f * dt;
		}
		if (kbd.KeyIsPressed('I')) {
			testCheck = true;
		}
		/*if (clock > 300) {
			if (!spawnCheck) {
				for (int i = 0; i < 20; i++) {
					entityHandler.AddEnemy(0.7f, { 15 + i, 75 + i });
					entityHandler.AddEnemy(0.7f, { 85 - i, 75 + i });
				}
				spawnCheck = true;
			}
			ExecuteCameraMovement(dt);
		}*/
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
		/*Pipeline<SurfaceDirectionalLighting>::camWorldX = xOffset;
		Pipeline<SurfaceDirectionalLighting>::camWorldZ = zOffset;*/
	}
	virtual void Draw() override {
		// clearing shared zbuffer between all pipelines per frame
		sceneZBuffer->Clear();
		// scene world and camera transformation matrices
		const Matf4 projectionMatrix = Matf4::Projection(4.0f, 3.0f, 1.0f, 100.0f);
		camRotInverse = Matf4::Identity() * Matf4::RotationY(camY) * Matf4::RotationX(camX);
		// reflection camRotInverse
		cameraPosition = { xOffset, yOffset, zOffset };
		reflectionCameraPosition = { xOffset, -yOffset, zOffset };
		reflectionCamRotInverse = Matf4::Identity() * Matf4::RotationY(camY+PI) * Matf4::RotationX(camX+PI);
		const Matf4 viewMatrix = Matf4::Translation(-cameraPosition) * camRotInverse;
		// reflection view Matrix
		const Matf4 reflectionViewMatrix = Matf4::Translation(-reflectionCameraPosition) * reflectionCamRotInverse;
		Vecf3 translate = { 0.0f, 0.0f, 0.0f };
		Matf4 worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		const Matf4 orientation = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y);
		// binding transformation matrices to external components and draws them
		// draws all entities in entity handler
		entityHandler.Draw(viewMatrix, projectionMatrix, reflectionViewMatrix);
		// draws world terrain and path found TerrainWithPath::FindPath()
		terrainWithPath->Draw(worldTransform, viewMatrix, projectionMatrix, reflectionViewMatrix);
		//entityHandler.DrawDebugDisplay();
		//terrainWithPath.DrawPath(viewMatrix, projectionMatrix);
		consoleBox->Draw(viewMatrix, projectionMatrix, reflectionViewMatrix);
		// drawing the water plane
		translate = { 0.0f, water->GetYOffSet(), 0.0f };
		worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		water->Draw(worldTransform, viewMatrix, projectionMatrix);
		worldViewProj = worldTransform * viewMatrix * projectionMatrix;
		CalculateUprightVector();

		//// debug gui test
		//int bufferSize = sceneZBuffer->width * sceneZBuffer->height;
		//int startX = 1;
		//int startY = 1;
		//int guiSize = 500;
		//for (int x = startX; x < guiSize; x++) {
		//	for (int y = startY; y < guiSize; y++) {
		//		int xInt = x*gfx.ScreenWidth/guiSize;
		//		int yInt = (guiSize-y)*gfx.ScreenHeight / guiSize;
		//		gfx.PutPixel(x, y, sceneZBuffer->ColorAt(xInt, yInt));
		//	}
		//}
		if (testCheck) {
			if (debugClock > 60) {
				consoleBox->Write(std::to_string(uprightVector.x) + "," + std::to_string(uprightVector.y) + "," + std::to_string(uprightVector.z));
				//consoleBox->Write(std::to_string(pointOnPlane.x) + "," + std::to_string(pointOnPlane.y) + "," + std::to_string(pointOnPlane.z));
				//consoleBox->Write(std::to_string(zOffset));
				debugClock = 0;
			}
			else {
				debugClock++;
			}
		}
	}
	void ExecuteCameraMovement(float dt) {
		cameraPosition += Vecf4{ 0.0f, 0.0f, 1.0f, 0.0f } * camSpeed * dt;
	}
	void CalculateUprightVector() {
		Matf4 rotationMatrix = Matf4::RotationZ(0) * Matf4::RotationX(camX) * Matf4::RotationY(camY);
		Vecf3 translatePoint = { xOffset, yOffset, zOffset };
		Matf4 pointWorldTransform = Matf4::RotationZ(0) * Matf4::RotationX(camX) * Matf4::RotationY(camY) * Matf4::Translation(translatePoint);
		Vecf3 vecHolder = (Vecf3)((Vecf4)worldVector * worldViewProj);
		Vecf3 pointHolder = (Vecf3)((Vecf4)worldPoint * worldViewProj);
		//pointHolder.x *= -1;
		pointOnPlane = pointHolder;
		vecHolder = (vecHolder - pointHolder).GetNormalized();
		/*vecHolder.x /= 2;
		vecHolder.z /= 2;*/
		uprightVector = vecHolder;
	}
//public:
//	// mouse interactivity
//	MouseInteract mouseInteract;
public:	
	static Vecf3 uprightVector;
	static Vecf3 pointOnPlane;
private:
	// shared zbuffer of scene
	std::shared_ptr<ZBuffer> sceneZBuffer;
	// orientation euler angles
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	float yOffset = 10.0f;
	float xOffset = -18.0f;
	float zOffset = -18.0f;
	// projection inverse matrices, directional light position, camera variables
	Matf4 camRotInverse;// = Matf4::Identity() * Matf4::RotationX(-0.8f) * Matf4::RotationY(camY);
	Matf4 reflectionCamRotInverse;// = Matf4::Identity() * Matf4::RotationX(0.8f) * Matf4::RotationY(camY);
	Vecf3 cameraPosition = { xOffset, yOffset, zOffset };
	Vecf3 reflectionCameraPosition = { xOffset, 0.0f, zOffset };
	Vecf3 lightPosition = { 0.0f, 0.0f, 0.6f };
	const float cameraSpeed = 4.0f;
	float camY = 0.0f;
	float camX = -3.14f/4;
	float camZ = 0.0f;
	float reflectionCamX = 0.0f;
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
	std::shared_ptr<Water> water;
	Matf4 worldViewProj = Matf4::Identity();

	// debugging stuff
	std::wstringstream ss;
	bool mouseAct = false;

	// cam scene variables
	float camSpeed = 0.5f;
	float rotationSpeed = 1.0f;
	bool rotate = false;
	bool rotateReverse = false;
	bool move = true;
	bool reverse = false;
	bool center = false;
	bool check = true;
	int clock = 0;
	bool spawnCheck = false;

	

	Vecf3 worldVector = { 0.0f, 2.0f, 0.0f };
	Vecf3 worldPoint = { 0.0f, 0.0f, 0.0f };
	int debugClock = 0;
	bool testCheck = true;

	// testing graphics
	Graphics& gfx;
};