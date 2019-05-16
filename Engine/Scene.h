#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "ZBuffer.h"
#include "MouseInteract.h"

#include <string>
#include <memory>

class Scene {
public:
	Scene(const std::string& name, std::shared_ptr<ZBuffer>& zBuffer, Graphics& gfx) 
		:
		name(name),
		mouseInteract(zBuffer, gfx)
	{}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw() = 0;
	virtual ~Scene() = default;
	const std::string& GetName() const {
		return name;
	}
public:
	// mouse interactivity
	MouseInteract mouseInteract;
private:
	std::string name;
};