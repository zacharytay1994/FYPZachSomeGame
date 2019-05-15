#pragma once

#include "Font.h"
#include "Keyboard.h"
#include "Pipeline.h"

#include <vector>
#include <string>
#include <stack>
#include <queue>

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
		Color color = Colors::Green;
		bool readColor = false;
		if (!lines.empty() && columnCount < (charPerColumn/2 - 3)) {
			// add index in stack to front of sentence
			for (char c : std::to_string(lineCount)) {
				font->PlaceFontColoured(surface, c, rowCount, columnCount, Colors::LightGray);
			}
			font->PlaceFontColoured(surface, '.', rowCount, columnCount, Colors::LightGray);
			rowCount++;

			// print out sentence
			for (char c : lines.top()) {
				charHolder = c;
				valRange = int(c);
				// check if word should be a different color
				if (valRange == 47) {
					readColor = true;
					continue;
				}
				if (readColor) {
					switch (valRange) {
					// color read
					case 114:
						color = Colors::Red;
						break;
					case 121:
						color = Colors::Yellow;
						break;
					case 98:
						color = Colors::Blue;
						break;
					case 103:
						color = Colors::Gray;
						break;
					}
					readColor = false;
					continue;
				}
				// evaluating top char in char buffer and placing font
				// if char is a-z
				if (valRange >= 97 && valRange <= 122) {
					font->PlaceFontColoured(surface, charHolder, rowCount, columnCount, color);
				}
				// if char is 0-9
				else if (valRange >= 48 && valRange <= 57) {
					font->PlaceFontColoured(surface, charHolder, rowCount, columnCount, color);
				}
				// if char is whitespace
				else if (charHolder == (' ')) {
					rowCount = ((rowCount + 1) % font->charPerRow);
					if (rowCount == 0) {
						columnCount++;
					}
					color = Colors::Green;
				}
				// if char is punctuation 33-47
				else if (valRange >= 33 && valRange <= 47) {
					font->PlaceFontColoured(surface, charHolder, rowCount, columnCount, color);
				}
				// if char is punctuation 58-64
				else if (valRange >= 58 && valRange <= 64) {
					font->PlaceFontColoured(surface, charHolder, rowCount, columnCount, color);
				}
				// if input is carriage return
				else if (valRange == 13) {
					rowCount = 0;
					columnCount++;
				}
			}
			pipeline->effect.pixelShader.BindSurface(surface);
			rowCount = 0;
			columnCount++;
		}
		else if (!(columnCount < (charPerColumn/2 - 3))) {
			surface = clearSurface;
			rowCount = 0;
			columnCount = 0;
			/*std::string holder = lines.top();
			lines = {};
			lines.push(holder);*/
			PrintStack();
		}
	}
	void AddSentence(const std::string& string) {
		lines.push(string);
		lineCount++;
	}
	void Write(const std::string& string) {
		AddSentence(string);
		PrintStack();
	}
	void CharPerFrame() {

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
	int lineCount = 0;
};