#pragma once

#include "Font.h"

class FontList {
public:
	FontList() 
		:
		CourierNew30by54(std::make_shared<Font>(Surface("fontsheet7.bmp"), 30, 54, 300, 600, 1500, 1500, 2, 2)),
		CourierNew15by27(std::make_shared<Font>(Surface("fontsheet8.bmp"), 15, 27, 300, 600, 1000, 1000, 2, 2)),
		LevelGUIGameLoop(std::make_shared<Font>(Surface("fontsheet8.bmp"), 15, 27, 150, 300, 750, 540, 3, 2)),
		LevelGUIDisplay(std::make_shared<Font>(Surface("fontsheet8.bmp"), 15, 27, 150, 300, 750, 360, 3, 2)),
		LevelGUIWrite(std::make_shared<Font>(Surface("fontsheet8.bmp"), 15, 27, 150, 300, 900, 27, 1, 0))
	{}
public:
	// current fonts
	std::shared_ptr<Font> CourierNew30by54;
	std::shared_ptr<Font> CourierNew15by27;
	std::shared_ptr<Font> LevelGUIGameLoop;
	std::shared_ptr<Font> LevelGUIDisplay;
	std::shared_ptr<Font> LevelGUIWrite;
};