#pragma once

// scenes, no inheritance testing prototype
#include "Scene.h"
// guis
#include "MenuSceneParent.h"

#include <memory>

class SceneGUIInterface {
public:
	SceneGUIInterface(std::vector<std::unique_ptr<Scene>>& scenes,
		std::stack<std::unique_ptr<MenuSceneParent>>& menuScenes)
		:
		scenes(scenes),
		menuScenes(menuScenes)
	{}
	void SendCurrentSceneMessage(const std::string& message) {
		*(scenes.begin)->ReceiveMessage(message);
	}
private:

	std::vector<std::unique_ptr<Scene>>& scenes;
	std::stack<std::unique_ptr<MenuSceneParent>>& menuScenes;
};