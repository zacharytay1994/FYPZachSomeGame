#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Rect.h"
#include "Surface.h"

#include <string>
#include <vector>

class MenuSceneParent {
public:
	MenuSceneParent(const std::string& name, const std::string& filename)
		:
		name(name),
		menuTexture(std::make_unique<Surface>(filename))
	{}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw() = 0;
	virtual void OnClick() = 0;
	virtual ~MenuSceneParent() = default;
	const std::string& GetName() const {
		return name;
	}
public:
	const std::unique_ptr<Surface> menuTexture;
private:
	const std::string name;
	const std::vector<Rect> clickableRects;
};
