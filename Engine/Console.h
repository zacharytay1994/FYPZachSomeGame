#pragma once

#include "Font.h"
#include "Keyboard.h"
#include "Pipeline.h"

#include <vector>
#include <string>
#include <stack>

template <class T>
class Console {
public:
	Console(const std::shared_ptr<Font>& font, std::shared_ptr<Pipeline<T>>& pipeline, const Surface& surface) 
		:
		font(font),
		pipeline(pipeline),
		surface(surface)
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
					//characterBuffer[(columnCount * charPerRow) + (rowCount - 1)] = charHolder;
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
	void PrintStack() {
		char charHolder;
		int valRange;
		if (!lines.empty() && columnCount < charPerColumn) {
			for (char c : lines.top()) {
				charHolder = c;
				valRange = int(c);
				// evaluating top char in char buffer and placing font
				if (charSize < charLimit) {
					// if char is a-z
					if (valRange >= 97 && valRange <= 122) {
						font->PlaceFont(surface, charHolder, rowCount, columnCount);
						//characterBuffer[(columnCount * charPerRow) + (rowCount - 1)] = charHolder;
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
					// if input is carriage return
					else if (valRange == 13) {
						rowCount = 0;
						columnCount++;
					}
					charSize++;
				}
			}
			rowCount = 0;
			columnCount++;
		}
		else if (!(columnCount < charPerColumn)) {
			surface = clearSurface;
			rowCount = 0;
			columnCount = 0;
			std::string holder = lines.top();
			lines = {};
			lines.push(holder);
			PrintStack();
		}
	}
	void AddSentence(const std::string& string) {
		lines.push(string);
	}
	void Write(const std::string& string) {
		AddSentence(string);
		PrintStack();
	}
private:
	const std::shared_ptr<Font>& font;
	std::shared_ptr<Pipeline<T>>& pipeline;
	// copied surface to be altered
	Surface surface;
	Surface clearSurface = surface;
	// clean surface
	//Surface cleanSurface = surface;
	const int charPerColumn = font->charPerColumn;
	const int charPerRow = font->charPerRow;
	const int charLimit = charPerRow * charPerColumn;
	int rowCount = 0;
	int columnCount = 0;
	int charSize = 0;
	// stack of lines to be written
	std::stack<std::string> lines;
};