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
#include "NDCTransformer.h"

#include <string>
#include <sstream>
#include <memory>
#include <chrono>
#include <ctime>

class DebugWorld : public Scene {
public:
	// size of the world in world space coordinates (worldSize by worldSize large)
	const float worldSize = 40.0f;
	// size of the world in terms of a grid (gridSize by gridSize large)
	const int gridSize = 100;
	// hence, worldSize/gridSize = density of vertices in world space
public:
	DebugWorld(Graphics& gfx, const std::shared_ptr<FontList> fontList)
		:
		sceneZBuffer(std::make_shared<ZBuffer>(gfx.ScreenWidth, gfx.ScreenHeight)),
		Scene("Debug world", sceneZBuffer, gfx),
		terrainWithPath(std::make_shared<TerrainWithPath>(gfx, sceneZBuffer, "heightmap2.bmp", "parchmentpaper.bmp", worldSize, gridSize, -3.0f, 3.0f)), // TerrainWithPath(graphics, zbuffer, heightmap, surface texture, world size, grid size, min world height, max world height)
		entityHandler(gfx, sceneZBuffer, worldSize, gridSize, terrainWithPath, consoleBox),
		consoleBox(std::make_shared<ConsoleBox>(gfx, sceneZBuffer, fontList)),
		water(std::make_shared<Water>(gfx, sceneZBuffer,0.0f)),
		gfx(gfx)
	{
		//entityHandler.AddSolid(1.0f, { 55, 0, 45 });
		// let entityHandler know about the heightmap to implicitly place some entities
		entityHandler.SetHeightMap(terrainWithPath->GetHeightMap());

		// ------------------ ADD ENTITIES TO WORLD -----------------------
		//entityHandler.AddEnemy(1.0f, { 8.0f, 0.1f, 8.0f });
		//entityHandler.AddBuilding(3.0f, { 15, 15 });
		entityHandler.AddBuilding(3.0f, { 0, 0 });
		entityHandler.AddBuilding(3.0f, { 30, 30 });
		entityHandler.AddBuilding(3.0f, { 70, 30 });
		entityHandler.AddBuilding(3.0f, { 30, 60 });
		entityHandler.AddBuilding(3.0f, { 50, 50 });
		entityHandler.AddBuilding(3.0f, { 55, 60 });
		
		//entityHandler.AddEnemy(1.0f, { 75, 75 });
		//entityHandler.AddEnemyAA(0.5f, { 75, 25 });
		//entityHandler.PopulateRandomTurrets(15);
		//entityHandler.AddTurret(2.5f, { 50, 50 });
		// -----------------------------------------------------------------

		// make known to world terrain of solid obstacle entities
		terrainWithPath->SyncWithWorldEntities(entityHandler.buildingBuffer);
	}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		clock++;
		// camera movement
		if (interactWorld) {
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
			if (kbd.KeyIsPressed('C')) {
				yOffset += cameraSpeed * dt;
			}
			if (kbd.KeyIsPressed('X')) {
				yOffset -= cameraSpeed * dt;
			}
			if (kbd.KeyIsPressed('Q')) {
				camY += rotationSpeed2 * dt;
			}
			if (kbd.KeyIsPressed('E')) {
				camY -= rotationSpeed2 * dt;
			}
		}
		/*if (kbd.KeyIsPressed('U')) {
			camX += rotationSpeed2 * dt;
		}
		if (kbd.KeyIsPressed('H')) {
			camX -= rotationSpeed2 * dt;
		}
		if (kbd.KeyIsPressed('L')) {
			camZ += rotationSpeed2 * dt;
		}*/
		if (kbd.KeyIsPressed('I')) {
			testCheck = true;
		}
		/*if (kbd.KeyIsPressed('B')) {
			camY += PI;
		}
		if (kbd.KeyIsPressed('V')) {
			camX += PI;
		}
		if (kbd.KeyIsPressed('G')) {
			yOffset = yOffset * -1;
		}
		if (kbd.KeyIsPressed('Z')) {
			camY = 0;
			camX = 0;
			yOffset = 10.0f;
		}*/
		if (kbd.KeyIsPressed('T')) {
			interactWorld = false;
		}
		if (kbd.KeyIsPressed(char(13))) {
			interactWorld = true;
		}
		if (kbd.KeyIsPressed('V')) {
			entityHandler.AddEnemy(1, Veci2(50, 75));
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
		//MoveForwardSlowly(dt);
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
		if (mouse.LeftIsPressed() /*&& mouseAct*/) {
			mouseInteract.SpawnEnemy(entityHandler, mouse);
			mouseAct = false;
		}
		//ss << test.x << "," << test.y << std::endl;
		OutputDebugString(ss.str().c_str());

		water->Update(dt);

		// process messages from gui
		/*if (ProcessMessages(frameMessage)) {
			if (frameMessage == "yes") {
				entityHandler.AddEnemy(0.5f, Veci2(50, 50));
			}
		}*/
		HandleFrameMessage();
	}
	virtual void Draw() override {
		// clearing shared zbuffer between all pipelines per frame
		sceneZBuffer->Clear();
		// scene world and camera transformation matrices
		const Matf4 projectionMatrix = Matf4::Projection(4.0f, 3.0f, 1.0f, 100.0f);

		// ---------------- REAL WORLD TRANSFORMATIONS ---------------------------
		// CAMERA TRANSFORMS
		// camera position
		cameraPosition = { xOffset, yOffset, zOffset };
		// camera inverse rotation, i.e. counter camera orientation
		camRotInverse = Matf4::RotationZ(camZ) * Matf4::RotationY(camY) * Matf4::RotationX(camX);
		// view matrix
		const Matf4 viewMatrix = Matf4::Translation(-cameraPosition) * camRotInverse;

		// MODEL TRANSFORMS
		// translation
		Vecf3 translate = { 0.0f, 0.0f, 0.0f };
		// world transform
		Matf4 worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		// model orientation, i.e. rotation about model center
		const Matf4 orientation = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y);

		// ---------------- REFLECTION WORLD TRANSFORMATIONS ---------------------
		// reflection camera position
		reflectionCameraPosition = { xOffset, -yOffset, zOffset };
		// reflection camRotInverse
		reflectionCamRotInverse = Matf4::RotationZ(camZ) * Matf4::RotationY(camY+PI) * Matf4::RotationX(camX+PI);
		// reflection view matrix
		const Matf4 reflectionViewMatrix = Matf4::Translation(-reflectionCameraPosition) * reflectionCamRotInverse;
		
		
		// --------------- BINDING TRANSFORMATIONS MATRICES TO EXTERNAL COMPONENTS FOR INVIDVIDUAL DRAWING -----------------
		// draws all entities in entity handler
		entityHandler.Draw(viewMatrix, projectionMatrix, reflectionViewMatrix);

		// draws world terrain and path found TerrainWithPath::FindPath()
		terrainWithPath->Draw(worldTransform, viewMatrix, projectionMatrix, reflectionViewMatrix);
		
		// draw skybox console
		consoleBox->Draw(viewMatrix, projectionMatrix, reflectionViewMatrix);

		// drawing the water plane
		translate = { 0.0f, water->GetYOffSet(), 0.0f };
		worldTransform = Matf4::RotationZ(theta_z) * Matf4::RotationX(theta_x) * Matf4::RotationY(theta_y) * Matf4::Translation(translate);
		water->Draw(worldTransform, viewMatrix, projectionMatrix, cameraPosition);

		// ------------- OTHER DRAW CALLS -------------------
		//entityHandler.DrawDebugDisplay();
		//terrainWithPath.DrawPath(viewMatrix, projectionMatrix);

		// debug gui test for quadtree visualization
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

		// interval skybox console display
		if (testCheck) {
			if (debugClock > 60) {
				// consoleBox->Write(STRING_HERE);
				debugClock = 0;
			}
			else {
				debugClock++;
			}
		}
	}
	void ExecuteCameraMovement(float dt) {
		cameraPosition += Vecf4{ 0.0f, 0.0f, 1.0f, 0.0f } * 10.0f * dt;
	}
	void MoveForwardSlowly(float dt) {
		zOffset += 2.0f * dt;
	}
	enum class Commands {
		createturret
	};
	// message to function conversion
	void HandleFrameMessage() {
		char parse = 'c';
		bool look = false;
		std::string command;
		std::string intParam;
		std::string stringParam;
		std::string boolParam;
		int intParamIndex = 0;
		int intParamArray[2];
		int stringParamIndex = 0;
		std::string stringParamArray[2];
		int boolParamIndex = 0;
		int boolParamArray[2];
		std::string frameMessage;
		std::wstringstream ss;
		if (!ProcessMessages(frameMessage)) {
			return;
		}
		for (char c : frameMessage) {
			if (look) {
				parse = c;
				look = false;
				continue;
			}
			if (c == '/') {
				look = true;
				continue;
			}
			if (c != ',') {
				if (parse == 'c') {
					command += c;
				}
				else if (parse == 'i') {
					intParam += c;
				}
				else  if (parse == 's') {
					stringParam += c;
				}
				else if (parse == 'b') {
					boolParam += c;
				}
			}
			else if (parse == 'i') {
				intParamArray[intParamIndex] = std::stoi(intParam);
				intParam = "";
				intParamIndex++;
			}
			else if (parse == 's') {
				stringParamArray[stringParamIndex] = stringParam;
				stringParam = "";
				stringParamIndex++;
			}
			else if (parse == 'b') {
				boolParamArray[boolParamIndex] = std::stoi(boolParam);
				boolParam = "";
				boolParamIndex++;
			}
		}
		// execute commands
		if (command.compare("createturret") == 0) {
			if (boolParamArray[0] == 1) {
				entityHandler.AddTurret((float)intParamArray[0], mouseInteract.GetGridPos(), true);
			}
			else if (boolParamArray[0] == 0) {
				entityHandler.AddTurret((float)intParamArray[0], mouseInteract.GetGridPos(), false);
			}
		}
		/*consoleBox->Write(command);
		consoleBox->Write("createturret");
		ss << intParamArray[0] << std::endl;
		OutputDebugString(ss.str().c_str());*/
	}
private:
	// shared zbuffer of scene
	std::shared_ptr<ZBuffer> sceneZBuffer;
	// orientation euler angles
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	float yOffset = 10.0f;
	float xOffset = 0.0f;
	float zOffset = -20.0f;
	// projection inverse matrices, directional light position, camera variables
	Matf4 camRotInverse;
	Matf4 reflectionCamRotInverse;
	Vecf3 cameraPosition;
	Vecf3 reflectionCameraPosition;
	Vecf3 lightPosition = { 0.0f, 0.0f, 0.6f };
	const float cameraSpeed = 40.0f;
	const float rotationSpeed2 = 10.0f;
	float camY = 0.0f;
	float camX = -PI/4.0f;
	float camZ = 0.0f;

	// EXTERNAL COMPONENTS
	// entityHandler object, handles all scene objects that inherit the entity class
	EntityHandler entityHandler;
	// object that represents the scene terrain and pathing grid, handles pathfinding and terrain rendering
	std::shared_ptr<TerrainWithPath> terrainWithPath;

	// environment object
	std::shared_ptr<ConsoleBox> consoleBox;
	std::shared_ptr<Water> water;
	Matf4 worldViewProj = Matf4::Identity();
	Matf4 worldViewProj2 = Matf4::Identity();

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

	// TESTING VARIABLES
	float testingval = 0.0f;
	int debugClock = 0;
	bool testCheck = false;
	// testing graphics
	Graphics& gfx;

	// gui interface
	std::string frameMessage;
	bool interactWorld = true;
};