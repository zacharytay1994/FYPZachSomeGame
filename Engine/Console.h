#pragma once

#include "Font.h"
#include "Keyboard.h"
#include "Pipeline.h"

#include <vector>

template <class T>
class Console {
public:
	Console(const std::shared_ptr<Font>& font, std::shared_ptr<Pipeline<T>>& pipeline, const Surface& surface) 
		:
		font(font),
		pipeline(pipeline),
		surface(surface),
		characterBuffer(std::vector<char>(charLimit, ' '))
	{}
	void Update(Keyboard& kbd) {
		if (!kbd.CharIsEmpty()) {
			const char charHolder = kbd.ReadChar();
			const int valRange = charHolder;
			// evaluating top char in char buffer and placing font
			if (charSize < charLimit) {
				// if char is a-z
				if (valRange >= 97 && valRange <= 122) {
					font->PlaceFont(surface, charHolder, rowCount, columnCount);
					characterBuffer[(columnCount * charPerRow) + (rowCount - 1)] = charHolder;
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
					/*characterBuffer[charCursor] = ' ';
					surface = cleanSurface;
					rowCount = 0;
					columnCount = 0;
					for (char c : characterBuffer) {
						if (c != ' ') {
							font->PlaceFont(surface, c, rowCount, columnCount);
						}
						else if (c == ' ') {
							rowCount = ((rowCount + 1) % font->charPerRow);
							if (rowCount == 0) {
								columnCount++;
							}
						}
					}
					pipeline->effect.pixelShader.BindSurface(surface);
					charCursor--;
					columnCount = (int)(charCursor / charPerColumn);
					rowCount = charCursor - columnCount;*/
				}
				//// up cursor row
				//else if (valRange == 91) {
				//	// setting row position
				//	if (charCursorY > 0) {
				//		charCursorY--;

				//		// getting column position
				//		for (int i = charPerRow; i > 0; i--) {
				//			if (characterBuffer[charCursorY * charPerRow + i] != ' ') {
				//				charCursor = charCursorY * charPerRow + i;
				//				break;
				//			}
				//			if (i == 0) {
				//				charCursor = charCursorY * charPerRow;
				//			}
				//		}
				//	}
				//}
				//else if (valRange == 93) {
				//	// setting row position
				//	if (charCursorY < charPerColumn) {
				//		charCursorY++;

				//		// getting column position
				//		for (int i = charPerRow; i > 0; i--) {
				//			if (characterBuffer[charCursorY * charPerRow + i] != ' ') {
				//				charCursor = charCursorY * charPerRow + i;
				//				break;
				//			}
				//			if (i == 0) {
				//				charCursor = charCursorY * charPerRow;
				//			}
				//		}
				//	}
				//}
				// if input is carriage return
				else if (valRange == 13) {
					rowCount = 0;
					columnCount++;
				}
				charSize++;
			}
		}
	}
	void InsertCharToBuffer(const char& c) {
		characterBuffer.push_back(c);
	}
	void RemoveCharFromBuffer(const char& c) {

	}
private:
	const std::shared_ptr<Font>& font;
	std::shared_ptr<Pipeline<T>>& pipeline;
	// copied surface to be altered
	Surface surface;
	// clean surface
	//Surface cleanSurface = surface;
	const int charPerColumn = font->charPerColumn;
	const int charPerRow = font->charPerRow;
	const int charLimit = charPerRow * charPerColumn;
	// 2D buffer row column 
	std::vector<char> characterBuffer;
	/*int charCursorX = 0;
	int charCursorY = 0;
	int charCursor = 0;*/
	int rowCount = 0;
	int columnCount = 0;
	int charSize = 0;
};