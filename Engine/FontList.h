#pragma once

#include "Font.h"

class FontList {
public:
	FontList() 
		:
		CourierNew30by54(std::make_shared<Font>(Surface("fontsheet7.bmp"), 30, 54, 300, 600, 1500, 1500, 2, 2))
	{}
public:
	// current fonts
	std::shared_ptr<Font> CourierNew30by54;
};