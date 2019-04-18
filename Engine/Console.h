#pragma once

#include "Font.h"
#include "Keyboard.h"
#include "Pipeline.h"

template <class T>
class Console {
public:
	Console(const std::shared_ptr<Font>& font, std::shared_ptr<Pipeline<T>>& pipeline) 
		:
		font(font),
		pipeline(pipeline)
	{}
	void Update(Surface& surface, Keyboard& kbd) {
		if (!kbd.CharIsEmpty()) {
			const char charHolder = kbd.ReadChar();
			const int valRange = charHolder;
			// evaluating top char in char buffer and placing font
			if (charSize < charLimit) {
				// if char is a-z
				if (valRange >= 97 && valRange <= 122) {
					font->PlaceFont(surface, charHolder, rowCount, columnCount);
					pipeline->effect.pixelShader.BindSurface(surface);
				}
				// if char is 0-9
				else if (valRange >= 48 && valRange <= 57) {
					font->PlaceFont(surface, charHolder, rowCount, columnCount);
					pipeline->effect.pixelShader.BindSurface(surface);
				}
				// if char is whitespace
				else if (charHolder == (' ')) {
					rowCount = ((rowCount + 1) % font->charPerRow);
					if (rowCount == 0) {
						columnCount++;
					}
				}
				// if char is punctuation 33-47
				else if (valRange >= 33 && valRange <= 47) {
					font->PlaceFont(surface, charHolder, rowCount, columnCount);
					pipeline->effect.pixelShader.BindSurface(surface);
				}
				// if char is punctuation 58-64
				else if (valRange >= 58 && valRange <= 64) {
					font->PlaceFont(surface, charHolder, rowCount, columnCount);
					pipeline->effect.pixelShader.BindSurface(surface);
				}
				// if input is backspace
				else if (valRange == 8) {

				}
				// if input is carriage return
				else if (valRange == 13) {
					rowCount = 0;
					columnCount++;
				}
				charSize++;
			}
		}
	}
private:
	const std::shared_ptr<Font>& font;
	std::shared_ptr<Pipeline<T>>& pipeline;
	int rowCount = 0;
	int columnCount = 0;
	int charSize = 0;
	const int charLimit = font->charPerRow * font->charPerColumn;
};