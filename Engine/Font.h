#pragma once

#include "Surface.h"

class Font {
public:
	Font(const Surface& fontsheet, const int& fontwidth, const int& fontheight,
		const int& fontPageWidth, const int& fontPageHeight, const int& surfaceOutWidth,
		const int& surfaceOutHeight, const int& paddingX, const int& paddingY)
		:
		fontSheet(fontsheet),
		fontWidth(fontwidth),
		fontHeight(fontheight),
		fontPageWidth(fontPageWidth),
		fontPageHeight(fontPageHeight),
		surfaceOutWidth(surfaceOutWidth),
		surfaceOutHeight(surfaceOutHeight),
		paddingX(paddingX),
		paddingY(paddingY)
	{}
	void PlaceFont(Surface& surfaceToAlter, const char& c, int& rowCount, int& columnCount) {
		// defined params needed
		const int xStart = rowCount * fontWidth + (paddingX * fontWidth);
		const int yStart = columnCount * fontHeight + (paddingY * fontHeight);
		const int xEnd = xStart + fontWidth;
		const int yEnd = yStart + fontHeight;
		int fontStartX;
		int fontStartY;
		// surface coords
		int x;
		int y;
		// fontsheet coords
		int i;
		int j;

		// ASCII conversion
		const int cVal = (int)c;
		fontStartX = (cVal - 97) * fontWidth;
		fontStartY = ((int)(cVal - 97)/10) * fontHeight;

		// loop in font pixels
		for (x = xStart, i = fontStartX; x < xEnd; x++, i++) {
			for (y = yStart, j = fontStartY; y < yEnd; y++, j++) {
				Color check = fontSheet.GetPixel(i, j);
				if ((Vecf3)check != Vecf3(255, 255, 255)) {
					surfaceToAlter.PutPixel(x, y, check);
				}
			}
		}
		rowCount = ((rowCount + 1) % charPerRow);
		if (rowCount == 0) {
			columnCount++;
		}
	}
public:
	const int fontWidth;
	const int fontHeight;
private:
	const Surface fontSheet;
	// dimensions of fontsheet
	const int fontPageWidth;
	const int fontPageHeight;
	// dimensions of output surface
	const int surfaceOutWidth;
	const int surfaceOutHeight;
	// padding X and Y in character number
	const int paddingX;
	const int paddingY;
public:
	// chacter per row
	const int charPerRow = (int)(surfaceOutWidth / fontWidth) - paddingX * 2;
	const int charPerColumn = (int)(surfaceOutHeight / fontHeight) - paddingY * 2;
};
